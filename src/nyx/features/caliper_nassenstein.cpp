#include "caliper.h"
#include "../parallel.h"
#include "rotation.h"

CaliperNassensteinFeature::CaliperNassensteinFeature() : FeatureMethod("CaliperNassensteinFeature")
{
	// Letting the feature dependency manager know
	provide_features({ STAT_NASSENSTEIN_DIAM_MIN,
			STAT_NASSENSTEIN_DIAM_MAX,
			STAT_NASSENSTEIN_DIAM_MEAN,
			STAT_NASSENSTEIN_DIAM_MEDIAN,
			STAT_NASSENSTEIN_DIAM_STDDEV,
			STAT_NASSENSTEIN_DIAM_MODE });

	// Features whose classes we want the Feature Manager to calculate prior to this feature class
	add_dependencies ({CONVEX_HULL_AREA});
}

void CaliperNassensteinFeature::calculate (LR& r)
{
	std::vector<double> allD;	// diameters at 0-180 degrees rotation
	calculate_diameters (r.convHull_CH, allD);

	auto s = ComputeCommonStatistics2(allD);
	_min = (double)s.min;
	_max = (double)s.max;
	_mean = s.mean;
	_median = s.median;
	_stdev = s.stdev;
	_mode = (double)s.mode;
}

void CaliperNassensteinFeature::save_value (std::vector<std::vector<double>>& fvals)
{
	fvals[STAT_NASSENSTEIN_DIAM_MIN][0] = _min;
	fvals[STAT_NASSENSTEIN_DIAM_MAX][0] = _max;
	fvals[STAT_NASSENSTEIN_DIAM_MEAN][0] = _mean;
	fvals[STAT_NASSENSTEIN_DIAM_MEDIAN][0] = _median;
	fvals[STAT_NASSENSTEIN_DIAM_STDDEV][0] = _stdev;
	fvals[STAT_NASSENSTEIN_DIAM_MODE][0] = _mode;
}

void CaliperNassensteinFeature::calculate_diameters (const std::vector<Pixel2>& convex_hull, std::vector<double>& all_D)
{
	// Rotated convex hull
	std::vector<Pixel2> CH_rot;
	CH_rot.reserve(convex_hull.size());

	// Rotate and calculate the diameter
	all_D.clear();
	for (float theta = 0.f; theta < 180.f; theta += rot_angle_increment)
	{
		Rotation::rotate_around_center(convex_hull, theta, CH_rot);
		auto [minX, minY, maxX, maxY] = AABB::from_pixelcloud(CH_rot);

		//
		std::vector<float> DA;	// Diameters at this angle

		// Iterate y-grid
		float stepY = (maxY - minY) / float(n_steps);
		for (int iy = 1; iy <= n_steps; iy++)
		{
			float chord_y = minY + iy * stepY;

			// Find convex hull segments intersecting 'y'
			std::vector<std::pair<float, float>> X;	// intersection points
			for (int iH = 1; iH < CH_rot.size(); iH++)
			{
				// The convex hull points are guaranteed to be consecutive
				auto& a = CH_rot[iH - 1],
					& b = CH_rot[iH];

				// Chord's Y is between segment AB's Ys ?
				if ((a.y >= chord_y && b.y <= chord_y) || (b.y >= chord_y && a.y <= chord_y))
				{
					auto chord_x = b.y != a.y ?
						(b.x - a.x) * (chord_y - a.y) / (b.y - a.y) + a.x
						: (b.y + a.y) / 2;
					auto tup = std::make_pair(chord_x, chord_y);
					X.push_back(tup);
				}
			}

			// Save the length of this chord. There must be 2 items in 'chordEnds' because we don't allow uniformative chords of zero length
			if (X.size() >= 2)
			{
				// for N segments
				auto compareFunc = [](const std::pair<float, float>& p1, const std::pair<float, float>& p2) { return p1.first < p2.first; };
				auto idx_minX = std::distance(X.begin(), std::min_element(X.begin(), X.end(), compareFunc));
				auto idx_maxX = std::distance(X.begin(), std::max_element(X.begin(), X.end(), compareFunc));
				// left X and right X segments
				auto& e1 = X[idx_minX], & e2 = X[idx_maxX];
				auto x1 = e1.first, y1 = e1.second, x2 = e2.first, y2 = e2.second;
				// save this chord
				auto dist = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);	// Squared distance
				DA.push_back(dist);
			}
		}

		if (DA.size() > 0)
		{
			// Find the shortest and longest chords (diameters)
			double minD2 = *std::min_element(DA.begin(), DA.end()),
				maxD2 = *std::max_element(DA.begin(), DA.end()),
				min_ = sqrt(minD2),
				max_ = sqrt(maxD2);

			// Save them
			all_D.push_back(min_);
			all_D.push_back(max_);
		}
	}
}

void CaliperNassensteinFeature::osized_calculate (LR& r, ImageLoader&)
{
	// Calculating this feature does not require access to the massive ROI pixel cloud, 
	// so we can reuse the trivial calculate()
	calculate(r);
}

void CaliperNassensteinFeature::parallel_process (std::vector<int>& roi_labels, std::unordered_map <int, LR>& roiData, int n_threads)
{
	size_t jobSize = roi_labels.size(),
		workPerThread = jobSize / n_threads;

	runParallel(CaliperNassensteinFeature::parallel_process_1_batch, n_threads, workPerThread, jobSize, &roi_labels, &roiData);
}

void CaliperNassensteinFeature::parallel_process_1_batch (size_t firstitem, size_t lastitem, std::vector<int>* ptrLabels, std::unordered_map <int, LR>* ptrLabelData)
{
	// Calculate the feature for each batch ROI item 
	for (auto i = firstitem; i < lastitem; i++)
	{
		// Get ahold of ROI's label and cache
		int roiLabel = (*ptrLabels) [i];
		LR& r = (*ptrLabelData) [roiLabel];

		// Calculate the feature and save it in ROI's csv-friendly buffer 'fvals'
		CaliperNassensteinFeature f;
		f.calculate(r);
		f.save_value(r.fvals);
	}
}
