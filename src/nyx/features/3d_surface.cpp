#include "../featureset.h"
#include "3d_surface.h"
#include "../parallel.h"

//??????? for dumping contours
#include "../environment.h"	
#include <regex>

#include "c:/WORK/AXLE/TEMP/hull_codes/MatLib.h"
#include "c:/WORK/AXLE/TEMP/hull_codes/convexhull.h"


bool D3_SurfaceFeature::required (const FeatureSet & fs)
{
	return fs.anyEnabled(
		{
			Nyxus::Feature3D::AREA,
			Nyxus::Feature3D::MESH_VOLUME,
			Nyxus::Feature3D::VOLUME_CONVEXHULL,
			Nyxus::Feature3D::DIAMETER_EQUAL_AREA,
			Nyxus::Feature3D::DIAMETER_EQUAL_VOLUME
		});
}

D3_SurfaceFeature::D3_SurfaceFeature() : FeatureMethod("D3_SurfaceFeature")
{
	provide_features(
		{
			Nyxus::Feature3D::AREA,
			Nyxus::Feature3D::MESH_VOLUME,
			Nyxus::Feature3D::VOLUME_CONVEXHULL,
			Nyxus::Feature3D::DIAMETER_EQUAL_AREA,
			Nyxus::Feature3D::DIAMETER_EQUAL_VOLUME		
		});
}

#define VERBOSLVL4(stmt) {}

void build_contour_imp_fat (
	// in
	std::vector <Pixel3> & contour, 
	// out
	const std::vector <Pixel3> & cloud,	// coordinates are not fixed for AABB
	const std::vector <size_t> & zplane, 
	int z, 
	int width, 
	int height, 
	int minx, 
	int miny)
{
	//==== Pad the image
	
	int paddingColor = 0;
	std::vector<PixIntens> paddedImage((height + 2) * (width + 2), paddingColor);
	for (auto idx : zplane) // 'zplane' is indices of the z-plane to build the contour for

	{
		auto & px = cloud[idx];
		auto x = px.x - minx + 1,
			y = px.y - miny + 1;
		paddedImage[x + y * (width + 2)] = idx/*px.inten*/ + 1;	// Decorate the intensity
	}

	VERBOSLVL4(
		std::cout << "\n\n\n" << "-- ContourFeature / buildRegularContour / Padded image --\n";
	for (int y = 0; y < height + 2; y++)
	{
		for (int x = 0; x < width + 2; x++)
		{
			size_t idx = x + y * (width + 2);
			auto inte = paddedImage[idx];
			if (inte)
				std::cout << '*';
			else
				std::cout << '.';
		}
		std::cout << "\n";
	}
	std::cout << "\n\n\n";
	);

	const int BLANK = 0;
	bool inside = false;
	int pos = 0;

	//==== Prepare the contour ("border") image
	std::vector<PixIntens> borderImage((height + 2) * (width + 2), 0);

	// Initialize the entire image to blank
	for (int y = 0; y < (height + 2); y++)
		for (int x = 0; x < (width + 2); x++)
			borderImage[x + y * (width + 2)] = BLANK;

	//==== Scan the padded image and fill the border one
	for (int y = 0; y < (height + 2); y++)
		for (int x = 0; x < (width + 2); x++)
		{
			pos = x + y * (width + 2);

			// Scan for a non-blank pixel
			if (borderImage[pos] != 0 && !inside)		// Entering an already discovered border
			{
				inside = true;
			}
			else if (paddedImage[pos] != 0 && inside)	// Already discovered border point
			{
				continue;
			}
			else if (paddedImage[pos] == BLANK && inside)	// Leaving a border
			{
				inside = false;
			}
			else if (paddedImage[pos] != 0 && !inside)	// Undiscovered border point
			{
				borderImage[pos] = paddedImage[pos];	// Non-blank

				int checkLocationNr = 1;	// The neighbor number of the location we want to check for a new border point
				int checkPosition;			// The corresponding absolute array address of checkLocationNr
				int newCheckLocationNr; 	// Variable that holds the neighborhood position we want to check if we find a new border at checkLocationNr
				int startPos = pos;			// Set start position
				int counter = 0; 			// Counter is used for the jacobi stop criterion
				int counter2 = 0; 			// Counter2 is used to determine if the point we have discovered is one single point

				// Defines the neighborhood offset position from current position and the neighborhood
				// position we want to check next if we find a new border at checkLocationNr
				int neighborhood[8][2] = {
						{-1,7},
						{-3 - width,7},
						{-width - 2,1},
						{-1 - width,1},
						{1,3},
						{3 + width,3},
						{width + 2,5},
						{1 + width,5}
				};

				// Trace around the neighborhood
				while (true)
				{
					checkPosition = pos + neighborhood[checkLocationNr - 1][0];
					newCheckLocationNr = neighborhood[checkLocationNr - 1][1];

					if (paddedImage[checkPosition] != 0) // Next border point found?
					{
						if (checkPosition == startPos)
						{
							counter++;

							// Stopping criterion (jacob)
							if (newCheckLocationNr == 1 || counter >= 3)
							{
								// Close loop
								inside = true; // Since we are starting the search at were we first started we must set inside to true
								break;
							}
						}

						checkLocationNr = newCheckLocationNr; // Update which neighborhood position we should check next
						pos = checkPosition;
						counter2 = 0; 						// Reset the counter that keeps track of how many neighbors we have visited
						borderImage[checkPosition] = paddedImage[checkPosition]; // Non-blank
					}
					else
					{
						// Rotate clockwise in the neighborhood
						checkLocationNr = 1 + (checkLocationNr % 8);
						if (counter2 > 8)
						{
							// If counter2 is above 8, we have sought around the neighborhood and
							// therefore the border is a single non-blank pixel, and we can exit
							counter2 = 0;
							break;
						}
						else
						{
							counter2++;
						}
					}
				}
			}
		}

	VERBOSLVL4(
		std::cout << "\n\n\n" << "-- ContourFeature / buildRegularContour / Contour image --\n";
	// header
	std::cout << "\t";	// indent
	for (int i = 0; i < width; i++)
		if (i % 10 == 0)
			std::cout << '|';
		else
			std::cout << '_';
	std::cout << "\n";
	//---
	for (int y = 0; y < height + 2; y++)
	{
		std::cout << "y=" << y << "\t";
		for (int x = 0; x < width + 2; x++)
		{
			size_t idx = x + y * (width + 2);
			auto inte = borderImage[idx];
			if (inte)
				std::cout << ' ';
			else
				std::cout << '+';
		}
		std::cout << "\n";
	}
	std::cout << "\n\n\n";
	);

	//==== Remove padding and save the contour image as a vector of non-blank pixels

	contour.clear();

	for (int y = 0; y < height + 2; y++)
		for (int x = 0; x < width + 2; x++)
		{
			size_t idx = x + y * (width + 2);
			auto ix = borderImage[idx];
			PixIntens inte = 0;
			if (ix)
				inte = cloud[ix-1].inten;
			if (inte)
			{
				Pixel3 p(x + minx, y + miny, z, inte); // indexed scenario: '..., z, inte-1'		// Cast pixel position from relative to absolute and undecorate its intensity
				contour.push_back(p);
			}
		}

#if 0
	//==== Reorder

	//	--containers for unordered (temp) and ordered (result) pixels
	std::list<Pixel2> unordered(r.contour.begin(), r.contour.end());
	std::vector<Pixel2> ordered;
	ordered.reserve(unordered.size());

	//	--initialize vector 'ordered' with 1st pixel of 'unordered'
	auto itBeg = unordered.begin();
	Pixel2 pxTip = *itBeg;
	ordered.push_back(pxTip);
	unordered.remove(pxTip);

	//	--tip of the ordered contour
	pxTip = ordered[0];

	//	--harvest items of 'unordered' 
	while (unordered.size())
	{
		//	--find the neighbor of the current tip pixel 
		std::vector<Pixel2> cands;	// candidates
		for (Pixel2& px : unordered)
		{
			//	--test for proximity and skip non-neighbors
			auto dx = std::fabs((int)px.x - (int)pxTip.x),
				dy = std::fabs((int)px.y - (int)pxTip.y);
			if (dx > 1 || dy > 1)
				continue;	// not a neighbor of pxTip

			//	--we found the neighbor; grab it; make it the new tip pixel; quit this search loop 
			cands.push_back(px);
		}

		//	--are there any tip's neighbor candidate?
		if (!cands.empty())
		{
			int distMin = pxTip.sqdist(cands[0]);
			int idxMin = 0;
			for (int i = 1; i < cands.size(); i++)
			{
				Pixel2& px = cands[i];
				int dist = pxTip.sqdist(cands[i]);
				if (dist < distMin)
				{
					idxMin = i;
					distMin = dist;
				}
			}
			Pixel2& px = cands[idxMin];
			// find the closest candidate to pxTip
			ordered.push_back(px);
			unordered.remove(px);
			pxTip = ordered[ordered.size() - 1];
		}
		else //	--any gaps left by the contour algorithm?
		{
			// Most likely unavailability of an immediate neighboring pixel is due to 
			// its sitting in the 'ordered' set already meaning that the contour is closed. 
			// Sometimes a contour is closed despite 'unordered' set is nonempty - such a 
			// redundancy is due to the Moore based algorithm above.
			VERBOSLVL4(
				std::cerr << "gap in contour!\n" << "tip pixel: " << pxTip.x << "," << pxTip.y << "\n";
			std::cerr << "ordered:\n";
			int i = 1;
			for (auto& pxo : ordered)
			{
				std::cerr << "\t" << pxo.x << "," << pxo.y;
				if (i++ % 10 == 0)
					std::cerr << "\n";
			}
			std::cerr << "\n";

			int neigR2 = 400;	// squared
			std::cerr << "unordered around the tip (R^2=" << neigR2 << "):\n";
			i = 1;
			for (auto& pxu : unordered)
			{
				// filter out the far neighborhood
				if (pxTip.sqdist(pxu) > neigR2)
					continue;

				std::cerr << "\t" << pxu.x << "," << pxu.y;
				if (i++ % 10 == 0)
					std::cerr << "\n";
			}
			std::cerr << "\n";

			std::cout << "\n\n\n" << "-- Contour image --\n";
			std::setw(3);
			// header
			std::cout << "\t";	// indent
			for (int i = 0; i < width; i++)
				if (i % 10 == 0)
					std::cout << '|';
				else
					std::cout << '_';
			std::cout << "\n";
			//---
			for (int y = 0; y < height + 2; y++)
			{
				std::cout << "y=" << y << "\t";
				for (int x = 0; x < width + 2; x++)
				{
					size_t idx = x + y * (width + 2);
					auto inte = borderImage[idx];

					// Display a pixel symbol depending on its role
					bool in_ordered = false;
					for (auto pxo : ordered)
						if (pxo.x == x && pxo.y == y)
						{
							in_ordered = true;
							break;
						}
					bool in_unordered = false;
					for (auto pxu : unordered)
						if (pxu.x == x && pxu.y == y)
						{
							in_unordered = true;
							break;
						}
					if (pxTip.x == x && pxTip.y == y)
						std::cout << 'T';
					else
						if (in_ordered)
						{
							if (ordered[0].x == x && ordered[0].y == y)
								std::cout << '1';
							else
								std::cout << 'O';
						}
						else
							if (in_unordered)
								std::cout << 'U';
							else
								if (inte == 0)
									std::cout << '+';
								else
									std::cout << ' ';

				}
				std::cout << "\n";
			}
			std::cout << "\n\n\n";
			);
			break;
		}
	}

	// replace the unordered contour with ordered one
	r.contour = ordered;
#endif
}

void build_contour_imp (
	// out
	std::vector <size_t>& contour,	// indices in the cloud
	// in
	const std::vector <Pixel3>& cloud,	// achtung! coordinates need fixing wrt AABB!
	const std::vector <size_t>& plane,
	int z,
	int width,
	int height,
	int minx,
	int miny)
{
	//==== Pad the image

	int paddingColor = 0;
	std::vector<PixIntens> paddedImage((height + 2) * (width + 2), paddingColor);
	for (auto idx : plane) // 'zplane' is indices of the z-plane to build the contour for

	{
		auto& px = cloud[idx];
		auto x = px.x - minx + 1,
			y = px.y - miny + 1;
		paddedImage[x + y * (width + 2)] = idx/*px.inten*/ + 1;	// Decorate the intensity
	}

	VERBOSLVL4(
		std::cout << "\n\n\n" << "-- ContourFeature / buildRegularContour / Padded image --\n";
	for (int y = 0; y < height + 2; y++)
	{
		for (int x = 0; x < width + 2; x++)
		{
			size_t idx = x + y * (width + 2);
			auto inte = paddedImage[idx];
			if (inte)
				std::cout << '*';
			else
				std::cout << '.';
		}
		std::cout << "\n";
	}
	std::cout << "\n\n\n";
	);

	const int BLANK = 0;
	bool inside = false;
	int pos = 0;

	//==== Prepare the contour ("border") image
	std::vector<PixIntens> borderImage((height + 2) * (width + 2), 0);

	// Initialize the entire image to blank
	for (int y = 0; y < (height + 2); y++)
		for (int x = 0; x < (width + 2); x++)
			borderImage[x + y * (width + 2)] = BLANK;

	//==== Scan the padded image and fill the border one
	for (int y = 0; y < (height + 2); y++)
		for (int x = 0; x < (width + 2); x++)
		{
			pos = x + y * (width + 2);

			// Scan for a non-blank pixel
			if (borderImage[pos] != 0 && !inside)		// Entering an already discovered border
			{
				inside = true;
			}
			else if (paddedImage[pos] != 0 && inside)	// Already discovered border point
			{
				continue;
			}
			else if (paddedImage[pos] == BLANK && inside)	// Leaving a border
			{
				inside = false;
			}
			else if (paddedImage[pos] != 0 && !inside)	// Undiscovered border point
			{
				borderImage[pos] = paddedImage[pos];	// Non-blank

				int checkLocationNr = 1;	// The neighbor number of the location we want to check for a new border point
				int checkPosition;			// The corresponding absolute array address of checkLocationNr
				int newCheckLocationNr; 	// Variable that holds the neighborhood position we want to check if we find a new border at checkLocationNr
				int startPos = pos;			// Set start position
				int counter = 0; 			// Counter is used for the jacobi stop criterion
				int counter2 = 0; 			// Counter2 is used to determine if the point we have discovered is one single point

				// Defines the neighborhood offset position from current position and the neighborhood
				// position we want to check next if we find a new border at checkLocationNr
				int neighborhood[8][2] = {
						{-1,7},
						{-3 - width,7},
						{-width - 2,1},
						{-1 - width,1},
						{1,3},
						{3 + width,3},
						{width + 2,5},
						{1 + width,5}
				};

				// Trace around the neighborhood
				while (true)
				{
					checkPosition = pos + neighborhood[checkLocationNr - 1][0];
					newCheckLocationNr = neighborhood[checkLocationNr - 1][1];

					if (paddedImage[checkPosition] != 0) // Next border point found?
					{
						if (checkPosition == startPos)
						{
							counter++;

							// Stopping criterion (jacob)
							if (newCheckLocationNr == 1 || counter >= 3)
							{
								// Close loop
								inside = true; // Since we are starting the search at were we first started we must set inside to true
								break;
							}
						}

						checkLocationNr = newCheckLocationNr; // Update which neighborhood position we should check next
						pos = checkPosition;
						counter2 = 0; 						// Reset the counter that keeps track of how many neighbors we have visited
						borderImage[checkPosition] = paddedImage[checkPosition]; // Non-blank
					}
					else
					{
						// Rotate clockwise in the neighborhood
						checkLocationNr = 1 + (checkLocationNr % 8);
						if (counter2 > 8)
						{
							// If counter2 is above 8, we have sought around the neighborhood and
							// therefore the border is a single non-blank pixel, and we can exit
							counter2 = 0;
							break;
						}
						else
						{
							counter2++;
						}
					}
				}
			}
		}

	VERBOSLVL4(
		std::cout << "\n\n\n" << "-- ContourFeature / buildRegularContour / Contour image --\n";
	// header
	std::cout << "\t";	// indent
	for (int i = 0; i < width; i++)
		if (i % 10 == 0)
			std::cout << '|';
		else
			std::cout << '_';
	std::cout << "\n";
	//---
	for (int y = 0; y < height + 2; y++)
	{
		std::cout << "y=" << y << "\t";
		for (int x = 0; x < width + 2; x++)
		{
			size_t idx = x + y * (width + 2);
			auto inte = borderImage[idx];
			if (inte)
				std::cout << ' ';
			else
				std::cout << '+';
		}
		std::cout << "\n";
	}
	std::cout << "\n\n\n";
	);

	//==== Remove padding and save the contour image as a vector of non-blank pixels

	contour.clear();

	for (int y = 0; y < height + 2; y++)
		for (int x = 0; x < width + 2; x++)
		{
			size_t idx = x + y * (width + 2);
			auto ix = borderImage[idx];
			PixIntens inte = 0;
			if (ix)
				inte = cloud[ix - 1].inten;
			if (inte)
			{
				//Pixel3 p(x + minx, y + miny, z, inte); // indexed scenario: '..., z, inte-1'		// Cast pixel position from relative to absolute and undecorate its intensity
				contour.push_back(ix-1); // push_back(p);
			}
		}
}

void dump_skinny_contour_3D (
	const std::vector <size_t>& K,	// contour	indices of contour voxels in the cloud
	const std::vector <Pixel3>& C,	// cloud	achtung! coordinates need fixing wrt AABB!
	int width,
	int height,
	int minx,
	int miny)
{
	if (K.size() == 0)
	{
		std::cout << "ERROR: blank contour\n";
		return;
	}

	// check if contour is on the same z-plane
	auto z0 = C[K[0]].z;
	for (auto ik : K)
		if (C[ik].z != z0)
		{
			std::cout << "ERROR: contour spans multiple z-planes: z_0=" << z0 << ", z_k=" << C[ik].z << "\n";
			return;
		}

	// sparse --> matrix
	PixIntens val0 = 0;
	std::vector<PixIntens> M (height*width, val0);
	for (auto ik : K)
	{
		auto& px = C [ik];
		auto x = px.x - minx,
			y = px.y - miny;
		M [x + y * width] = px.inten;
	}

	// display it
	std::cout << "\n\n\n" << "-- Skinny contour --\n";
	// header
	std::cout << "\t";	// indent
	for (int i = 0; i < width; i++)
		if (i % 10 == 0)
			std::cout << '|';
		else
			std::cout << '_';
	std::cout << "\n";
	//---
	for (int y = 0; y < height; y++)
	{
		std::cout << "y=" << y << "\t";
		for (int x = 0; x < width; x++)
		{
			size_t idx = x + y * width;
			auto inte = M[idx];
			if (inte)
				std::cout << ' ';
			else
				std::cout << '+';
		}
		std::cout << "\n";
	}
	std::cout << "\n\n\n";
}

void dump_contours_3D (LR & r)
{
	//std::unordered_map<int, std::vector<Pixel3>> & ZK

	static int fileNumber = 1;

	std::string fpath;
	fpath = Nyxus::theEnvironment.output_dir + '/' + "3dcontours-" + std::to_string(fileNumber++) + ".csv";

	std::cout << ">>> dumping 3d contour --> " << fpath << "\n";

	std::ofstream ofile;
	ofile.open(fpath);
	ofile << "label,x,y,z,inten\n";

	std::cout << "\t" << r.contours_3D.size() << " z\n";
	for (auto& plane : r.contours_3D)
		for (auto ip : plane.second)
		{
			auto p = r.raw_pixels_3D[ip];
			ofile << r.label << ',' << p.x << ',' << p.y << ',' << p.z << ',' << p.inten << '\n';
		}

	ofile.close();
}

struct Simplex
{
	// indices of simplex vertices in the point cloud
	size_t a, b, c;
};

//
// H - hull
// C - cloud
// KK - z-resolved contours
//
void build_convex_hull_3D (std::vector<Simplex> & H, const std::vector<Pixel3> & C, const std::unordered_map<int, std::vector<size_t>> & KK)
{

	Mat* V = readmat((char*)"C:\\WORK\\AXLE\\data\\OUTPUT-3D\\3dcontour-1.csv");
	//showmat(V);
	convexhull* cvh = convh(V);
	writemat((char*)"C:\\WORK\\AXLE\\TEMP\\hull_codes\\2dcloud_facets.txt", cvh->facets);
	writemat((char*)"C:\\WORK\\AXLE\\TEMP\\hull_codes\\2dcloud_neighbors_indices.txt", cvh->neighbors_indices);
	writemat((char*)"C:\\WORK\\AXLE\\TEMP\\hull_codes\\2dcloud_outpoints_indices.txt", cvh->outpoints_indices);
	::free(V);

	/*
	// count surface voxels
	size_t N = 0;
	for (auto& k_plane : KK)
		N += k_plane.second.size();

	// allocate
	Mat* A = newmat (N, 3, 0);

	// initialize
	size_t k = 0;
	for (auto& k_plane : KK)
	{
		for (auto i : k_plane.second)
		{
			auto& p = C[i];
			A->entries[k++] = p.x;
			A->entries[k++] = p.y;
			A->entries[k++] = p.z;
		}
	}

	// get the hull
	convexhull* cvh = convh (A);
//	writemat((char*)"C:\\WORK\\AXLE\\TEMP\\hull_codes\\2dcloud_facets.txt", cvh->facets);
//	writemat((char*)"C:\\WORK\\AXLE\\TEMP\\hull_codes\\2dcloud_neighbors_indices.txt", cvh->neighbors_indices);
//	writemat((char*)"C:\\WORK\\AXLE\\TEMP\\hull_codes\\2dcloud_outpoints_indices.txt", cvh->outpoints_indices);
//	::free(V);

	// free
	freemat(A);
	*/

}

void D3_SurfaceFeature::calculate(LR& r)
{
	// calculate contours
	r.contours_3D.clear();

	for (auto& plane : r.zplanes)
	{
		// fat contour
		//		std::vector<Pixel3> K;
		//		build_contour_imp_fat (K, r.raw_pixels_3D, plane.second, plane.first, r.aabb.get_width(), r.aabb.get_height(), r.aabb.get_xmin(), r.aabb.get_ymin());

		// skinny contour
		std::vector<size_t> K;	// indices in the cloud
		build_contour_imp (K, r.raw_pixels_3D, plane.second, plane.first, r.aabb.get_width(), r.aabb.get_height(), r.aabb.get_xmin(), r.aabb.get_ymin());

		// store it
		r.contours_3D[plane.first] = K;
	}

	// dump ROI's contours
	//		for (auto& zpln : r.contours_3D)
	//			dump_skinny_contour_3D(zpln.second, r.raw_pixels_3D, r.aabb.get_width(), r.aabb.get_height(), r.aabb.get_xmin(), r.aabb.get_ymin());

	// dump
	dump_contours_3D(r);


	// calculate the convex hull
	std::vector<Simplex> H;
	build_convex_hull_3D (H, r.raw_pixels_3D, r.contours_3D);
}

void D3_SurfaceFeature::osized_add_online_pixel(size_t x, size_t y, uint32_t intensity) {}

void D3_SurfaceFeature::osized_calculate(LR& r, ImageLoader& imloader)
{
}

void D3_SurfaceFeature::save_value(std::vector<std::vector<double>>& fvals)
{
	fvals[(int)Nyxus::Feature3D::AREA][0] = fval_AREA;
	fvals[(int)Nyxus::Feature3D::MESH_VOLUME][0] = fval_VOLUME;
	fvals[(int)Nyxus::Feature3D::VOLUME_CONVEXHULL][0] = fval_VOLUME_CONVEXHULL;
	fvals[(int)Nyxus::Feature3D::DIAMETER_EQUAL_AREA][0] = fval_DIAMETER_EQUAL_AREA;
	fvals[(int)Nyxus::Feature3D::DIAMETER_EQUAL_VOLUME][0] = fval_DIAMETER_EQUAL_VOLUME;
}

void D3_SurfaceFeature::cleanup_instance()
{
	fval_AREA = 0;
	fval_VOLUME = 0;
	fval_VOLUME_CONVEXHULL = 0;
	fval_DIAMETER_EQUAL_AREA = 0;
	fval_DIAMETER_EQUAL_VOLUME = 0;
}

void D3_SurfaceFeature::parallel_process (std::vector<int>& roi_labels, std::unordered_map <int, LR>& roiData, int n_threads)
{
	size_t jobSize = roi_labels.size(),
		workPerThread = jobSize / n_threads;

	Nyxus::runParallel (D3_SurfaceFeature::parallel_process_1_batch, n_threads, workPerThread, jobSize, &roi_labels, &roiData);
}

void D3_SurfaceFeature::parallel_process_1_batch (size_t firstitem, size_t lastitem, std::vector<int>* ptrLabels, std::unordered_map <int, LR>* ptrLabelData)
{
	// Calculate the feature for each batch ROI item 
	for (auto i = firstitem; i < lastitem; i++)
	{
		// Get ahold of ROI's label and cache
		int roiLabel = (*ptrLabels)[i];
		LR& r = (*ptrLabelData)[roiLabel];

		// Skip the ROI if its data is invalid to prevent nans and infs in the output
		if (r.has_bad_data())
			continue;

		// Calculate the feature and save it in ROI's csv-friendly buffer 'fvals'
		D3_SurfaceFeature f;
		f.calculate(r);
		f.save_value(r.fvals);
	}
}

void D3_SurfaceFeature::reduce(size_t start, size_t end, std::vector<int>* ptrLabels, std::unordered_map <int, LR>* ptrLabelData)
{
	for (auto i = start; i < end; i++)
	{
		int lab = (*ptrLabels)[i];
		LR& r = (*ptrLabelData)[lab];
		D3_SurfaceFeature f;
		f.calculate(r);
		f.save_value(r.fvals);
	}
}


