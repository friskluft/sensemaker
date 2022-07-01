#include "../roi_cache.h"

void LR::reduce_pixel_intensity_features()
{
	LR& lr = *this;
	for (auto& pxl : lr.raw_pixels)
	{
		auto intensity = pxl.inten;
		auto x = pxl.x, y = pxl.y;

		// Count of pixels belonging to the label
		auto prev_n = lr.raw_pixels.size();	// Previous count
		lr.aux_PrevCount = prev_n;
		auto n = prev_n + 1;	// New count
		//lr.pixelCountRoiArea = n;

		// Cumulants for moments calculation
		auto prev_mean = lr.fvals[MEAN][0]; //lr.mean;
		auto delta = intensity - prev_mean;
		auto delta_n = delta / n;
		auto delta_n2 = delta_n * delta_n;
		auto term1 = delta * delta_n * prev_n;

		// Mean
		auto mean = prev_mean + delta_n;

		// Moments
		lr.aux_M4 = lr.aux_M4 + term1 * delta_n2 * (n * n - 3 * n + 3) + 6 * delta_n2 * lr.aux_M2 - 4 * delta_n * lr.aux_M3;
		lr.aux_M3 = lr.aux_M3 + term1 * delta_n * (n - 2) - 3 * delta_n * lr.aux_M2;
		lr.aux_M2 = lr.aux_M2 + term1;

		// Min 
		lr.fvals[MIN][0] = std::min(lr.fvals[MIN][0], (StatsReal)intensity); // lr.min = std::min(lr.min, (StatsInt)intensity);

		// Max
		lr.fvals[MAX][0] = std::max(lr.fvals[MAX][0], (StatsReal)intensity); // lr.max = std::max(lr.max, (StatsInt)intensity);

		// Energy
		lr.fvals[ENERGY][0] = lr.fvals[ENERGY][0] + intensity * intensity; // lr.massEnergy = lr.massEnergy + intensity * intensity;

		// Variance and standard deviation
		if (n >= 2)
		{
			double s_prev = lr.aux_variance,
				diff = double(intensity) - prev_mean,
				diff2 = diff * diff;
			lr.aux_variance = (n - 2) * s_prev / (n - 1) + diff2 / n;
		}
		else
			lr.aux_variance = 0;

		// Mean absolute deviation
		lr.fvals[MEAN_ABSOLUTE_DEVIATION][0] = lr.fvals[MEAN_ABSOLUTE_DEVIATION][0] + std::abs(intensity - mean); // lr.MAD = lr.MAD + std::abs(intensity - mean);

		// Weighted centroids. Needs reduction. Do we need to make them 1-based for compatibility with Matlab and WNDCHRM?
		lr.fvals[CENTROID_X][0] = lr.fvals[CENTROID_X][0] + StatsReal(x); // lr.centroid_x = lr.centroid_x + StatsReal(x);
		lr.fvals[CENTROID_Y][0] = lr.fvals[CENTROID_Y][0] + StatsReal(y); // lr.centroid_y = lr.centroid_y + StatsReal(y);

		#if 0	// Replaced with a faster version (class TrivialHistogram)
		// Histogram
		auto ptrH = lr.aux_Histogram;
		ptrH->add_observation(intensity);
		#endif

		// Previous intensity for succeeding iterations
		lr.aux_PrevIntens = intensity;
	}
}



