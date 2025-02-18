#pragma once

#pragma once
#include "../featureset.h"
#include "../feature_method.h"

/// @brief A contour is a vector of X and Y coordinates of all the pixels on the border of a ROI. This class uses Moore's algorithm for cnotour detection.
class D3_SurfaceFeature : public FeatureMethod
{
public:
	D3_SurfaceFeature();
	void calculate(LR& r);
	void osized_add_online_pixel(size_t x, size_t y, uint32_t intensity);
	void osized_calculate(LR& r, ImageLoader& imloader);
	void save_value(std::vector<std::vector<double>>& feature_vals);
	void parallel_process(std::vector<int>& roi_labels, std::unordered_map <int, LR>& roiData, int n_threads);
	static void parallel_process_1_batch(size_t firstitem, size_t lastitem, std::vector<int>* ptrLabels, std::unordered_map <int, LR>* ptrLabelData);
	void cleanup_instance();
	static void reduce(size_t start, size_t end, std::vector<int>* ptrLabels, std::unordered_map <int, LR>* ptrLabelData);

	static bool required(const FeatureSet& fs);

private:
	void buildSurface (LR& r);
	void buildSurface_nontriv(LR& r);
	double
		fval_AREA = 0,
		fval_VOLUME = 0,
		fval_VOLUME_CONVEXHULL = 0,
		fval_DIAMETER_EQUAL_AREA = 0,
		fval_DIAMETER_EQUAL_VOLUME = 0;
};

