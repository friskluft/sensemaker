#include <string>
#include <vector>
#include <map>
#include <array>
#ifdef WITH_PYTHON_H
#include <pybind11/pybind11.h>
#endif
#include "environment.h"
#include "globals.h"
#include "helpers/timing.h"

namespace Nyxus
{
	void feed_pixel_2_cache(int x, int y, int label, PixIntens intensity)
	{
		// Update basic ROI info (info that doesn't require costly calculations)
		LR& r = roiData[label];
		r.raw_pixels.push_back(Pixel2(x, y, intensity));
	}

	bool scanTrivialRois (const std::vector<int>& batch_labels, const std::string& intens_fpath, const std::string& label_fpath, int num_FL_threads)
	{
		// Sort the batch's labels to enable binary searching in it
		std::vector<int> whiteList = batch_labels;
		std::sort (whiteList.begin(), whiteList.end());

		int lvl = 0,	// Pyramid level
			lyr = 0;	//	Layer

		// Read the tiffs
		size_t nth = theImLoader.get_num_tiles_hor(),
			ntv = theImLoader.get_num_tiles_vert(),
			fw = theImLoader.get_tile_width(),
			th = theImLoader.get_tile_height(),
			tw = theImLoader.get_tile_width(),
			tileSize = theImLoader.get_tile_size(),
			fullwidth = theImLoader.get_full_width(),
			fullheight = theImLoader.get_full_height();

		int cnt = 1;
		for (unsigned int row = 0; row < nth; row++)
			for (unsigned int col = 0; col < ntv; col++)
			{
				// Fetch the tile 
				bool ok = theImLoader.load_tile(row, col);
				if (!ok)
				{
					std::stringstream ss;
					ss << "Error fetching tile row=" << row << " col=" << col;
					#ifdef WITH_PYTHON_H
						throw ss.str();
					#endif	
					std::cerr << ss.str() << "\n";
					return false;
				}

				// Get ahold of tile's pixel buffer
				auto dataI = theImLoader.get_int_tile_buffer(),
					dataL = theImLoader.get_seg_tile_buffer();

				// Iterate pixels
				for (unsigned long i = 0; i < tileSize; i++)
				{
					// Skip non-mask pixels
					auto label = dataL[i];
					if (! label)
						continue;

					// Skip this ROI if the label isn't in the pending set of a multi-ROI mode
					if (! theEnvironment.singleROI && ! std::binary_search(whiteList.begin(), whiteList.end(), label)) //--slow-- if (std::find(PendingRoiLabels.begin(), PendingRoiLabels.end(), label) == PendingRoiLabels.end())
						continue;

					auto inten = dataI[i];
					int y = row * th + i / tw,
						x = col * tw + i % tw;

					// Skip tile buffer pixels beyond the image's bounds
					if (x >= fullwidth || y >= fullheight)
						continue;

					// Collapse all the labels to one if single-ROI mde is requested
					if (theEnvironment.singleROI)
						label = 1;

					// Cache this pixel 
					feed_pixel_2_cache (x, y, label, dataI[i]);
				}

				// Show stayalive progress info
				if (cnt++ % 4 == 0)
					VERBOSLVL1(std::cout << "\tscan trivial " << int((row * nth + col) * 100 / float(nth * ntv) * 100) / 100. << "% of image scanned \n";)
			}

		return true;
	}

	void allocateTrivialRoisBuffers(const std::vector<int>& Pending)
	{
		for (auto lab : Pending)
		{
			LR& r = roiData [lab];
			r.aux_image_matrix.use_roi (r.raw_pixels, r.aabb);
		}
	}

	void freeTrivialRoisBuffers(const std::vector<int>& Pending)
	{
		for (auto lab : Pending)
		{
			LR& r = roiData[lab];
			
			r.raw_pixels.clear();
			r.raw_pixels.shrink_to_fit();

			r.aux_image_matrix.clear();		// clear() calls then std::vector::shrink_to_fit()
		}
	}

	bool processTrivialRois (const std::vector<int>& trivRoiLabels, const std::string& intens_fpath, const std::string& label_fpath, int num_FL_threads, size_t memory_limit)
	{
		std::vector<int> Pending;
		size_t batchDemand = 0;
		int roiBatchNo = 1;

		for (auto lab : trivRoiLabels)
		{
			LR& r = roiData[lab];

			size_t itemFootprint = r.get_ram_footprint_estimate();

			// Sheck if we are good to accumulate this ROI in the current batch or should close the batch and reduce it
			if (batchDemand + itemFootprint < memory_limit)
			{
				Pending.push_back(lab);
				batchDemand += itemFootprint;
			}
			else
			{
				// Scan pixels of pending trivial ROIs 
				std::sort (Pending.begin(), Pending.end());
				VERBOSLVL1(std::cout << ">>> Scanning batch #" << roiBatchNo << " of " << Pending.size() << " pending ROIs of " << uniqueLabels.size() << " all ROIs\n";)
				VERBOSLVL1(
					if (Pending.size() ==1)					
						std::cout << ">>> (single ROI " << Pending[0] << ")\n";
					else
						std::cout << ">>> (ROIs " << Pending[0] << " ... " << Pending[Pending.size() - 1] << ")\n";
					)
				scanTrivialRois(Pending, intens_fpath, label_fpath, num_FL_threads);

				// Allocate memory
				VERBOSLVL1(std::cout << "\tallocating ROI buffers\n";)
				allocateTrivialRoisBuffers (Pending);

				// Reduce them
				VERBOSLVL1(std::cout << "\treducing ROIs\n";)
				// reduce_trivial_rois(Pending);	
				reduce_trivial_rois_manual(Pending);

				// Output results
				//outputRoisFeatures (Pending);

				// Free memory
				VERBOSLVL1(std::cout << "\tfreeing ROI buffers\n";)
				freeTrivialRoisBuffers (Pending);	// frees what's allocated by feed_pixel_2_cache() and allocateTrivialRoisBuffers()

				// Reset the RAM footprint accumulator
				batchDemand = 0;

				// Clear the freshly processed ROIs from pending list 
				Pending.clear();

				// Start a new pending set by adding the stopper ROI 
				Pending.push_back(lab);

				// Advance the batch counter
				roiBatchNo++;
			}

			// Allow heyboard interrupt.

#ifdef WITH_PYTHON_H
			if (PyErr_CheckSignals() != 0)
                throw pybind11::error_already_set();
#endif
		}

		// Process what's remaining pending
		if (Pending.size() > 0)
		{
			// Scan pixels of pending trivial ROIs 
			std::sort (Pending.begin(), Pending.end());
			VERBOSLVL1(std::cout << ">>> Scanning batch #" << roiBatchNo << " of " << Pending.size() << " pending ROIs of " << uniqueLabels.size() << " all ROIs\n";)
			VERBOSLVL1(
				if (Pending.size() == 1)
					std::cout << ">>> (single ROI " << Pending[0] << ")\n";
				else
					std::cout << ">>> (ROIs " << Pending[0] << " ... " << Pending[Pending.size() - 1] << ")\n";
				)
			scanTrivialRois(Pending, intens_fpath, label_fpath, num_FL_threads);

			// Allocate memory
			VERBOSLVL1(std::cout << "\tallocating ROI buffers\n";)
			allocateTrivialRoisBuffers(Pending);

			// Reduce them
			VERBOSLVL1(std::cout << "\treducing ROIs\n";)
			//reduce_trivial_rois(Pending);	
			reduce_trivial_rois_manual(Pending);

			// Output results
			//outputRoisFeatures(Pending);

			// Free memory
			VERBOSLVL1(std::cout << "\tfreeing ROI buffers\n";)
			freeTrivialRoisBuffers(Pending);

			#ifdef WITH_PYTHON_H
			// Allow heyboard interrupt.
			if (PyErr_CheckSignals() != 0)
                throw pybind11::error_already_set();
			#endif
		}

		return true;
	}

}
