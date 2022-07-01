#include "../environment.h"
#include "../parallel.h"
#include "histogram.h"
#include "intensity.h"
#include "pixel.h"


PixelIntensityFeatures::PixelIntensityFeatures() : FeatureMethod("PixelIntensityFeatures")
{
	provide_features({ 
		INTEGRATED_INTENSITY,
		MEAN,
		MEDIAN,
		MIN,
		MAX,
		RANGE,
		STANDARD_DEVIATION,
		STANDARD_ERROR,
		SKEWNESS,
		KURTOSIS,
		HYPERSKEWNESS,
		HYPERFLATNESS,
		MEAN_ABSOLUTE_DEVIATION,
		ENERGY,
		ROOT_MEAN_SQUARED,
		ENTROPY,
		MODE,
		UNIFORMITY,
		UNIFORMITY_PIU,
		P01, P10, P25, P75, P90, P99,
		INTERQUARTILE_RANGE,
		ROBUST_MEAN_ABSOLUTE_DEVIATION
		});
}

void PixelIntensityFeatures::calculate(LR& r)
{
	// --MIN, MAX
	val_MIN  = r.aux_min;
	val_MAX = r.aux_max;

	double n = r.aux_area;

	// --AREA
	//val_AREA_PIXELS_COUNT = n;
	//if (theEnvironment.xyRes > 0.0)
	//		val_AREA_UM2  = n * std::pow(theEnvironment.pixelSizeUm, 2);

	// --MEAN, ENERGY, CENTROID_XY
	double mean_ = 0.0;
	double energy = 0.0;
	double cen_x = 0.0,
		cen_y = 0.0,
		integInten = 0.0;
	for (auto& px : r.raw_pixels)
	{
		mean_ += px.inten;
		energy += px.inten * px.inten;
		cen_x += px.x;
		cen_y += px.y;
		integInten += px.inten;
	}
	mean_ /= n;
	val_MEAN = mean_;
	val_ENERGY = energy;
	val_ROOT_MEAN_SQUARED = sqrt(val_ENERGY / n);
	val_INTEGRATED_INTENSITY = integInten;

	// --Centroid and Compactness
	//val_CENTROID_X = cen_x;
	//val_CENTROID_Y = cen_y;
	//
	//Moments2 mom2;
	//for (auto& px : lr.raw_pixels)
	//{
	//	double dst = std::sqrt(px.sqdist(cen_x, cen_y));
	//	mom2.add(dst);
	//}
	//double compa = mom2.std() / n;
	//val_COMPACTNESS = compa;

	// --MAD, VARIANCE, STDDEV
	double mad = 0.0,
		var = 0.0;
	for (auto& px : r.raw_pixels)
	{
		mad += std::abs(px.inten - mean_);
		var += (px.inten - mean_) * (px.inten - mean_);
	}
	val_MEAN_ABSOLUTE_DEVIATION = mad / n;
	var /= n;
	double stddev = sqrt(var);
	val_STANDARD_DEVIATION = stddev;

	// --Standard error
	val_STANDARD_ERROR = stddev / sqrt(n);

	//==== Do not calculate features of all-blank intensities (to avoid NANs)
	if (r.aux_min==0 && r.aux_max==0)
		return;

	// P10, 25, 75, 90, IQR, RMAD, entropy, uniformity
	TrivialHistogram H;
	H.initialize (val_MIN, val_MAX, r.raw_pixels);
	auto [median_, mode_, p01_, p10_, p25_, p75_, p90_, p99_, iqr_, rmad_, entropy_, uniformity_] = H.get_stats();
	val_MEDIAN = median_;
	val_P01 = p01_;
	val_P10 = p10_;
	val_P25 = p25_;
	val_P75 = p75_;
	val_P90 = p90_;
	val_P99 = p99_;
	val_INTERQUARTILE_RANGE = iqr_;
	val_ROBUST_MEAN_ABSOLUTE_DEVIATION = rmad_;
	val_ENTROPY = entropy_;
	val_MODE = mode_;
	val_UNIFORMITY = uniformity_;

	// --Uniformity calculated as PIU, percent image uniformity - see "A comparison of five standard methods for evaluating image intensity uniformity in partially parallel imaging MRI" [https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3745492/] and https://aapm.onlinelibrary.wiley.com/doi/abs/10.1118/1.2241606
	double piu = (1.0 - double(r.aux_max - r.aux_min) / double(r.aux_max + r.aux_min)) * 100.0;
	val_UNIFORMITY_PIU = piu;

	// Skewness
	//--Formula 1--	lr.fvals[SKEWNESS][0] = std::sqrt(n) * lr.aux_M3 / std::pow(lr.aux_M2, 1.5);
	//--Formula 2-- skewness = 3 * (mean - median) / stddev
	Moments4 mom;
	for (auto& px : r.raw_pixels)
		mom.add(px.inten);
	val_SKEWNESS = mom.skewness();

	// Kurtosis
	//--Formula-- k1 = mean((x - mean(x)). ^ 4) / std(x). ^ 4
	val_KURTOSIS = mom.kurtosis();

	// Hyperskewness hs = E[x-mean].^5 / std(x).^5
	val_HYPERSKEWNESS = mom.hyperskewness();

	// Hyperflatness hf = E[x-mean].^6 / std(x).^6
	val_HYPERFLATNESS = mom.hyperflatness();

	//==== Basic morphology :: Bounding box
	//	val_BBOX_XMIN = lr.aabb.get_xmin();
	//	val_BBOX_YMIN = lr.aabb.get_ymin();
	//	val_BBOX_WIDTH = lr.aabb.get_width();
	//	val_BBOX_HEIGHT = lr.aabb.get_height();

	//==== Basic morphology :: Centroids
	//	lr.fvals[CENTROID_X][0] = lr.fvals[CENTROID_Y][0] = 0.0;
	//	for (auto& px : lr.raw_pixels)
	//	{
	//			lr.fvals[CENTROID_X][0] += px.x;
	//			lr.fvals[CENTROID_Y][0] += px.y;
	//	}
	//	lr.fvals[CENTROID_X][0] /= n;
	//	lr.fvals[CENTROID_Y][0] /= n;

	////==== Basic morphology :: Weighted centroids
	//double x_mass = 0, y_mass = 0, mass = 0;

	//for (auto& px : lr.raw_pixels)
	//{
	//	x_mass = x_mass + (px.x + 1) * px.inten;    // the "+1" is only for compatability with matlab code (where index starts from 1) 
	//	y_mass = y_mass + (px.y + 1) * px.inten;    // the "+1" is only for compatability with matlab code (where index starts from 1) 
	//	mass += px.inten;
	//}

	//if (mass > 0)
	//{
	//	lr.fvals[WEIGHTED_CENTROID_X][0] = x_mass / mass;
	//	lr.fvals[WEIGHTED_CENTROID_Y][0] = y_mass / mass;
	//}
	//else
	//{
	//	lr.fvals[WEIGHTED_CENTROID_X][0] = 0.0;
	//	lr.fvals[WEIGHTED_CENTROID_Y][0] = 0.0;
	//}

	//// --Mass displacement (The distance between the centers of gravity in the gray-level representation of the object and the binary representation of the object.)
	//double dx = lr.fvals[WEIGHTED_CENTROID_X][0] - lr.fvals[CENTROID_X][0],
	//	dy = lr.fvals[WEIGHTED_CENTROID_Y][0] - lr.fvals[CENTROID_Y][0],
	//	dist = std::sqrt(dx * dx + dy * dy);
	//lr.fvals[MASS_DISPLACEMENT][0] = dist;

	////==== Basic morphology :: Extent
	//lr.fvals[EXTENT][0] = n / lr.aabb.get_area();

	////==== Basic morphology :: Aspect ratio
	//lr.fvals[ASPECT_RATIO][0] = lr.aabb.get_width() / lr.aabb.get_height();
}

void PixelIntensityFeatures::osized_add_online_pixel(size_t x, size_t y, uint32_t intensity)
{}

void PixelIntensityFeatures::osized_calculate (LR& r, ImageLoader& imloader)
{
	// --MIN, MAX
	val_MIN = r.aux_min;
	val_MAX = r.aux_max;

	double n = r.aux_area;

	// --MEAN, ENERGY, CENTROID_XY
	double mean_ = 0.0;
	double energy = 0.0;
	double cen_x = 0.0,
		cen_y = 0.0,
		integInten = 0.0;
	for (size_t i = 0; i < n; i++) //--- for (auto& px : r.raw_pixels)
	{
		Pixel2 px = r.osized_pixel_cloud.get_at(i);
		mean_ += px.inten;
		energy += px.inten * px.inten;
		cen_x += px.x;
		cen_y += px.y;
		integInten += px.inten;
	}
	mean_ /= n;
	val_MEAN = mean_;
	val_ENERGY = energy;
	val_ROOT_MEAN_SQUARED = sqrt(val_ENERGY / n);
	val_INTEGRATED_INTENSITY = integInten;

	// --MAD, VARIANCE, STDDEV
	double mad = 0.0,
		var = 0.0;
	for (auto& px : r.raw_pixels)
	{
		mad += std::abs(px.inten - mean_);
		var += (px.inten - mean_) * (px.inten - mean_);
	}
	val_MEAN_ABSOLUTE_DEVIATION = mad / n;
	var /= n;
	double stddev = sqrt(var);
	val_STANDARD_DEVIATION = stddev;

	// --Standard error
	val_STANDARD_ERROR = stddev / sqrt(n);

	//==== Do not calculate features of all-blank intensities (to avoid NANs)
	if (r.aux_min == 0 && r.aux_max == 0)
		return;

	// P10, 25, 75, 90, IQR, RMAD, entropy, uniformity
	TrivialHistogram H;
	H.initialize(val_MIN, val_MAX, r.raw_pixels);
	auto [median_, mode_, p01_, p10_, p25_, p75_, p90_, p99_, iqr_, rmad_, entropy_, uniformity_] = H.get_stats();
	val_MEDIAN = median_;
	val_P01 = p01_;
	val_P10 = p10_;
	val_P25 = p25_;
	val_P75 = p75_;
	val_P90 = p90_;
	val_P99 = p99_;
	val_INTERQUARTILE_RANGE = iqr_;
	val_ROBUST_MEAN_ABSOLUTE_DEVIATION = rmad_;
	val_ENTROPY = entropy_;
	val_MODE = mode_;
	val_UNIFORMITY = uniformity_;

	// --Uniformity calculated as PIU, percent image uniformity - see "A comparison of five standard methods for evaluating image intensity uniformity in partially parallel imaging MRI" [https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3745492/] and https://aapm.onlinelibrary.wiley.com/doi/abs/10.1118/1.2241606
	double piu = (1.0 - double(r.aux_max - r.aux_min) / double(r.aux_max + r.aux_min)) * 100.0;
	val_UNIFORMITY_PIU = piu;

	// Skewness
	//--Formula 1--	lr.fvals[SKEWNESS][0] = std::sqrt(n) * lr.aux_M3 / std::pow(lr.aux_M2, 1.5);
	//--Formula 2-- skewness = 3 * (mean - median) / stddev
	Moments4 mom;
	for (size_t i = 0; i < n; i++) //--- for (auto& px : r.raw_pixels)
	{
		Pixel2 px = r.osized_pixel_cloud.get_at(i);
		mom.add(px.inten);
	}
	val_SKEWNESS = mom.skewness();

	// Kurtosis
	//--Formula-- k1 = mean((x - mean(x)). ^ 4) / std(x). ^ 4
	val_KURTOSIS = mom.kurtosis();

	// Hyperskewness hs = E[x-mean].^5 / std(x).^5
	val_HYPERSKEWNESS = mom.hyperskewness();

	// Hyperflatness hf = E[x-mean].^6 / std(x).^6
	val_HYPERFLATNESS = mom.hyperflatness();
}

void PixelIntensityFeatures::save_value(std::vector<std::vector<double>>& fvals)
{
	fvals [INTEGRATED_INTENSITY][0] = val_INTEGRATED_INTENSITY; 
	fvals[MEAN][0] = val_MEAN;
	fvals[MEDIAN][0] = val_MEDIAN;
	fvals[MIN][0] = val_MIN;
	fvals[MAX][0] = val_MAX;
	fvals[RANGE][0] = val_RANGE;
	fvals[STANDARD_DEVIATION][0] = val_STANDARD_DEVIATION;
	fvals[STANDARD_ERROR][0] = val_STANDARD_ERROR;
	fvals[SKEWNESS][0] = val_SKEWNESS;
	fvals[KURTOSIS][0] = val_KURTOSIS;
	fvals[HYPERSKEWNESS][0] = val_HYPERSKEWNESS;
	fvals[HYPERFLATNESS][0] = val_HYPERFLATNESS;
	fvals[MEAN_ABSOLUTE_DEVIATION][0] = val_MEAN_ABSOLUTE_DEVIATION;
	fvals[ENERGY][0] = val_ENERGY;
	fvals[ROOT_MEAN_SQUARED][0] = val_ROOT_MEAN_SQUARED;
	fvals[ENTROPY][0] = val_ENTROPY;
	fvals[MODE][0] = val_MODE;
	fvals[UNIFORMITY][0] = val_UNIFORMITY;
	fvals[UNIFORMITY_PIU][0] = val_UNIFORMITY_PIU;
	fvals[P01][0] = val_P01;
	fvals[P10][0] = val_P10;
	fvals[P25][0] = val_P25;
	fvals[P75][0] = val_P75;
	fvals[P90][0] = val_P90;
	fvals[P99][0] = val_P99;
	fvals[INTERQUARTILE_RANGE][0] = val_INTERQUARTILE_RANGE;
	fvals[ROBUST_MEAN_ABSOLUTE_DEVIATION][0] = val_ROBUST_MEAN_ABSOLUTE_DEVIATION;
}

void PixelIntensityFeatures::parallel_process(std::vector<int>& roi_labels, std::unordered_map <int, LR>& roiData, int n_threads)
{
	size_t jobSize = roi_labels.size(),
		workPerThread = jobSize / n_threads;

	runParallel(PixelIntensityFeatures::parallel_process_1_batch, n_threads, workPerThread, jobSize, &roi_labels, &roiData);
}

void PixelIntensityFeatures::parallel_process_1_batch(size_t firstitem, size_t lastitem, std::vector<int>* ptrLabels, std::unordered_map <int, LR>* ptrLabelData)
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
		PixelIntensityFeatures f;
		f.calculate(r);
		f.save_value(r.fvals);
	}
}

void PixelIntensityFeatures::cleanup_instance()
{
	val_INTEGRATED_INTENSITY = 0,
		val_MEAN = 0,
		val_MEDIAN = 0,
		val_MIN = 0,
		val_MAX = 0,
		val_RANGE = 0,
		val_STANDARD_DEVIATION = 0,
		val_STANDARD_ERROR = 0,
		val_SKEWNESS = 0,
		val_KURTOSIS = 0,
		val_HYPERSKEWNESS = 0,
		val_HYPERFLATNESS = 0,
		val_MEAN_ABSOLUTE_DEVIATION = 0,
		val_ENERGY = 0,
		val_ROOT_MEAN_SQUARED = 0,
		val_ENTROPY = 0,
		val_MODE = 0,
		val_UNIFORMITY = 0,
		val_UNIFORMITY_PIU = 0,
		val_P01 = 0, val_P10 = 0, val_P25 = 0, val_P75 = 0, val_P90 = 0, val_P99 = 0,
		val_INTERQUARTILE_RANGE = 0,
		val_ROBUST_MEAN_ABSOLUTE_DEVIATION = 0;
}

