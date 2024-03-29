#include <map>
#include <string>
#include "featureset.h"
#include "roi_cache.h"

namespace Nyxus
{
	std::map <std::string, AvailableFeatures> UserFacingFeatureNames =
	{
		//=== 2D features

		// Pixel intensity stats
		{"INTEGRATED_INTENSITY", INTEGRATED_INTENSITY},
		{"MEAN", MEAN},
		{"MEDIAN", MEDIAN },
		{"MIN", MIN},
		{"MAX",MAX},
		{"RANGE",RANGE},
		{"STANDARD_DEVIATION",STANDARD_DEVIATION},
		{"STANDARD_ERROR", STANDARD_ERROR},
		{"SKEWNESS",SKEWNESS},
		{"KURTOSIS",KURTOSIS},
		{"HYPERSKEWNESS", HYPERSKEWNESS},
		{"HYPERFLATNESS", HYPERFLATNESS},
		{"MEAN_ABSOLUTE_DEVIATION",MEAN_ABSOLUTE_DEVIATION},
		{"ENERGY",ENERGY},
		{"ROOT_MEAN_SQUARED",ROOT_MEAN_SQUARED},
		{"ENTROPY",ENTROPY},
		{"MODE",MODE},
		{"UNIFORMITY",UNIFORMITY},
		{"UNIFORMITY_PIU", UNIFORMITY_PIU},
		{"P01", P01},
		{"P10", P10},
		{"P25", P25},
		{"P75", P75},
		{"P90",P90},
		{"P99", P99},
		{"INTERQUARTILE_RANGE",INTERQUARTILE_RANGE},
		{"ROBUST_MEAN_ABSOLUTE_DEVIATION",ROBUST_MEAN_ABSOLUTE_DEVIATION},
		{"WEIGHTED_CENTROID_Y",WEIGHTED_CENTROID_Y},
		{"WEIGHTED_CENTROID_X",WEIGHTED_CENTROID_X},
		{"MASS_DISPLACEMENT", MASS_DISPLACEMENT},

		// Morphology:
		{"AREA_PIXELS_COUNT", AREA_PIXELS_COUNT},
		{"AREA_UM2", AREA_UM2},
		{"CENTROID_X", CENTROID_X},
		{"CENTROID_Y", CENTROID_Y},
		{"COMPACTNESS", COMPACTNESS},
		{"BBOX_YMIN", BBOX_YMIN},
		{"BBOX_XMIN", BBOX_XMIN},
		{"BBOX_HEIGHT", BBOX_HEIGHT},
		{"BBOX_WIDTH", BBOX_WIDTH},
		{"DIAMETER_EQUAL_AREA", DIAMETER_EQUAL_AREA},

		// Ellipse fitting:
		{"MAJOR_AXIS_LENGTH", MAJOR_AXIS_LENGTH},
		{"MINOR_AXIS_LENGTH", MINOR_AXIS_LENGTH},
		{"ECCENTRICITY", ECCENTRICITY},
		{"ELONGATION", ELONGATION},
		{"ORIENTATION", ORIENTATION},
		{"ROUNDNESS", ROUNDNESS},

		{"NUM_NEIGHBORS",NUM_NEIGHBORS},
		{"PERCENT_TOUCHING", PERCENT_TOUCHING},
		{"CLOSEST_NEIGHBOR1_DIST", CLOSEST_NEIGHBOR1_DIST},
		{"CLOSEST_NEIGHBOR1_ANG", CLOSEST_NEIGHBOR1_ANG},
		{"CLOSEST_NEIGHBOR2_DIST", CLOSEST_NEIGHBOR2_DIST},
		{"CLOSEST_NEIGHBOR2_ANG", CLOSEST_NEIGHBOR2_ANG},
		{"ANG_BW_NEIGHBORS_MEAN", ANG_BW_NEIGHBORS_MEAN},
		{"ANG_BW_NEIGHBORS_STDDEV", ANG_BW_NEIGHBORS_STDDEV},
		{"ANG_BW_NEIGHBORS_MODE", ANG_BW_NEIGHBORS_MODE},

		{"EXTENT",EXTENT},
		{"ASPECT_RATIO",ASPECT_RATIO},

		{"DIAMETER_EQUAL_PERIMETER", DIAMETER_EQUAL_PERIMETER },
		{"EDGE_MEAN_INTENSITY", EDGE_MEAN_INTENSITY },
		{"EDGE_STDDEV_INTENSITY", EDGE_STDDEV_INTENSITY},
		{"EDGE_MAX_INTENSITY", EDGE_MAX_INTENSITY},
		{"EDGE_MIN_INTENSITY", EDGE_MIN_INTENSITY},

		{"CONVEX_HULL_AREA",CONVEX_HULL_AREA},
		{"SOLIDITY",SOLIDITY},

		{"PERIMETER",PERIMETER},
		{"EDGE_MEAN_INTENSITY",PERIMETER},
		{"EDGE_STDDEV_INTENSITY",PERIMETER},
		{"EDGE_MAX_INTENSITY",PERIMETER},
		{"EDGE_MIN_INTENSITY",PERIMETER},
		{"EDGE_INTEGRATED_INTENSITY", EDGE_INTEGRATED_INTENSITY},	

		{"CIRCULARITY", CIRCULARITY},

		{"EROSIONS_2_VANISH", EROSIONS_2_VANISH},
		{"EROSIONS_2_VANISH_COMPLEMENT", EROSIONS_2_VANISH_COMPLEMENT},

		{"FRACT_DIM_BOXCOUNT", FRACT_DIM_BOXCOUNT},
		{"FRACT_DIM_PERIMETER", FRACT_DIM_PERIMETER},

		{"EXTREMA_P1_X", EXTREMA_P1_X},
		{"EXTREMA_P1_Y",EXTREMA_P1_Y},
		{"EXTREMA_P2_X", EXTREMA_P2_X},
		{"EXTREMA_P2_Y",EXTREMA_P2_Y},
		{"EXTREMA_P3_X", EXTREMA_P3_X},
		{"EXTREMA_P3_Y",EXTREMA_P3_Y},
		{"EXTREMA_P4_X", EXTREMA_P4_X},
		{"EXTREMA_P4_Y",EXTREMA_P4_Y},
		{"EXTREMA_P5_X", EXTREMA_P5_X},
		{"EXTREMA_P5_Y",EXTREMA_P5_Y},
		{"EXTREMA_P6_X", EXTREMA_P6_X},
		{"EXTREMA_P6_Y",EXTREMA_P6_Y},
		{"EXTREMA_P7_X", EXTREMA_P7_X},
		{"EXTREMA_P7_Y",EXTREMA_P7_Y},
		{"EXTREMA_P8_X", EXTREMA_P8_X},
		{"EXTREMA_P8_Y",EXTREMA_P8_Y},

		{"MIN_FERET_DIAMETER",MIN_FERET_DIAMETER},
		{"MAX_FERET_DIAMETER",MAX_FERET_DIAMETER},
		{"MIN_FERET_ANGLE",MIN_FERET_ANGLE},
		{"MAX_FERET_ANGLE",MAX_FERET_ANGLE},

		{"STAT_FERET_DIAM_MIN",STAT_FERET_DIAM_MIN},
		{"STAT_FERET_DIAM_MAX",STAT_FERET_DIAM_MAX},
		{"STAT_FERET_DIAM_MEAN",STAT_FERET_DIAM_MEAN},
		{"STAT_FERET_DIAM_MEDIAN",STAT_FERET_DIAM_MEDIAN},
		{"STAT_FERET_DIAM_STDDEV",STAT_FERET_DIAM_STDDEV},
		{"STAT_FERET_DIAM_MODE",STAT_FERET_DIAM_MODE},

		{"STAT_MARTIN_DIAM_MIN",STAT_MARTIN_DIAM_MIN},
		{"STAT_MARTIN_DIAM_MAX",STAT_MARTIN_DIAM_MAX},
		{"STAT_MARTIN_DIAM_MEAN",STAT_MARTIN_DIAM_MEAN},
		{"STAT_MARTIN_DIAM_MEDIAN",STAT_MARTIN_DIAM_MEDIAN},
		{"STAT_MARTIN_DIAM_STDDEV",STAT_MARTIN_DIAM_STDDEV},
		{"STAT_MARTIN_DIAM_MODE",STAT_MARTIN_DIAM_MODE},

		{"STAT_NASSENSTEIN_DIAM_MIN",STAT_NASSENSTEIN_DIAM_MIN},
		{"STAT_NASSENSTEIN_DIAM_MAX",STAT_NASSENSTEIN_DIAM_MAX},
		{"STAT_NASSENSTEIN_DIAM_MEAN",STAT_NASSENSTEIN_DIAM_MEAN},
		{"STAT_NASSENSTEIN_DIAM_MEDIAN",STAT_NASSENSTEIN_DIAM_MEDIAN},
		{"STAT_NASSENSTEIN_DIAM_STDDEV",STAT_NASSENSTEIN_DIAM_STDDEV},
		{"STAT_NASSENSTEIN_DIAM_MODE",STAT_NASSENSTEIN_DIAM_MODE},

		{"MAXCHORDS_MAX", MAXCHORDS_MAX},
		{ "MAXCHORDS_MAX_ANG", MAXCHORDS_MAX_ANG},
		{ "MAXCHORDS_MIN", MAXCHORDS_MIN},
		{ "MAXCHORDS_MIN_ANG", MAXCHORDS_MIN_ANG},
		{ "MAXCHORDS_MEDIAN", MAXCHORDS_MEDIAN},
		{ "MAXCHORDS_MEAN", MAXCHORDS_MEAN},
		{ "MAXCHORDS_MODE", MAXCHORDS_MODE},
		{ "MAXCHORDS_STDDEV", MAXCHORDS_STDDEV},
		{ "ALLCHORDS_MAX", ALLCHORDS_MAX},
		{ "ALLCHORDS_MAX_ANG", ALLCHORDS_MAX_ANG},
		{ "ALLCHORDS_MIN", ALLCHORDS_MIN},
		{ "ALLCHORDS_MIN_ANG", ALLCHORDS_MIN_ANG},
		{ "ALLCHORDS_MEDIAN", ALLCHORDS_MEDIAN},
		{ "ALLCHORDS_MEAN", ALLCHORDS_MEAN},
		{ "ALLCHORDS_MODE", ALLCHORDS_MODE},
		{ "ALLCHORDS_STDDEV", ALLCHORDS_STDDEV},

		{"EULER_NUMBER",EULER_NUMBER},

		{"POLYGONALITY_AVE",POLYGONALITY_AVE},
		{"HEXAGONALITY_AVE",HEXAGONALITY_AVE},
		{"HEXAGONALITY_STDDEV",HEXAGONALITY_STDDEV},

		{"DIAMETER_MIN_ENCLOSING_CIRCLE",DIAMETER_MIN_ENCLOSING_CIRCLE},
		{"DIAMETER_CIRCUMSCRIBING_CIRCLE",DIAMETER_CIRCUMSCRIBING_CIRCLE },
		{"DIAMETER_INSCRIBING_CIRCLE",DIAMETER_INSCRIBING_CIRCLE },
		{"GEODETIC_LENGTH",GEODETIC_LENGTH },
		{"THICKNESS",THICKNESS },

		{ "ROI_RADIUS_MEAN", ROI_RADIUS_MEAN },
		{ "ROI_RADIUS_MAX", ROI_RADIUS_MAX },
		{ "ROI_RADIUS_MEDIAN", ROI_RADIUS_MEDIAN },

		{"GLCM_ANGULAR2NDMOMENT", GLCM_ANGULAR2NDMOMENT },
		{"GLCM_CONTRAST", GLCM_CONTRAST },
		{"GLCM_CORRELATION", GLCM_CORRELATION },
		{"GLCM_VARIANCE", GLCM_VARIANCE },
		{"GLCM_INVERSEDIFFERENCEMOMENT", GLCM_INVERSEDIFFERENCEMOMENT },
		{"GLCM_SUMAVERAGE", GLCM_SUMAVERAGE },
		{"GLCM_SUMVARIANCE", GLCM_SUMVARIANCE },
		{"GLCM_SUMENTROPY", 	GLCM_SUMENTROPY },
		{"GLCM_ENTROPY", GLCM_ENTROPY },
		{"GLCM_DIFFERENCEVARIANCE", GLCM_DIFFERENCEVARIANCE },
		{"GLCM_DIFFERENCEENTROPY", GLCM_DIFFERENCEENTROPY },
		{"GLCM_INFOMEAS1", GLCM_INFOMEAS1 },
		{"GLCM_INFOMEAS2", GLCM_INFOMEAS2 },

		{"GLRLM_SRE", GLRLM_SRE },
		{"GLRLM_LRE", GLRLM_LRE },
		{"GLRLM_GLN", GLRLM_GLN},
		{"GLRLM_GLNN", GLRLM_GLNN},
		{"GLRLM_RLN", GLRLM_RLN},
		{"GLRLM_RLNN", GLRLM_RLNN},
		{"GLRLM_RP", GLRLM_RP},
		{"GLRLM_GLV", GLRLM_GLV},
		{"GLRLM_RV", GLRLM_RV},
		{"GLRLM_RE", GLRLM_RE},
		{"GLRLM_LGLRE", GLRLM_LGLRE},
		{"GLRLM_HGLRE", GLRLM_HGLRE},
		{"GLRLM_SRLGLE", GLRLM_SRLGLE},
		{"GLRLM_SRHGLE", GLRLM_SRHGLE},
		{"GLRLM_LRLGLE", GLRLM_LRLGLE},
		{"GLRLM_LRHGLE", GLRLM_LRHGLE},

		{"GLSZM_SAE", GLSZM_SAE},
		{"GLSZM_LAE", GLSZM_LAE},
		{"GLSZM_GLN", GLSZM_GLN },
		{"GLSZM_GLNN", GLSZM_GLNN },
		{"GLSZM_SZN", GLSZM_SZN },
		{"GLSZM_SZNN", GLSZM_SZNN },
		{"GLSZM_ZP", GLSZM_ZP },
		{"GLSZM_GLV", GLSZM_GLV },
		{"GLSZM_ZV", GLSZM_ZV },
		{"GLSZM_ZE", GLSZM_ZE },
		{"GLSZM_LGLZE", GLSZM_LGLZE },
		{"GLSZM_HGLZE", GLSZM_HGLZE },
		{"GLSZM_SALGLE", GLSZM_SALGLE },
		{"GLSZM_SAHGLE", GLSZM_SAHGLE },
		{"GLSZM_LALGLE", GLSZM_LALGLE},
		{"GLSZM_LAHGLE", GLSZM_LAHGLE},

		{ "GLDM_SDE", GLDM_SDE },
		{ "GLDM_LDE", GLDM_LDE },
		{ "GLDM_GLN", GLDM_GLN },
		{ "GLDM_DN", GLDM_DN },
		{ "GLDM_DNN", GLDM_DNN },
		{ "GLDM_GLV", GLDM_GLV },
		{ "GLDM_DV", GLDM_DV },
		{ "GLDM_DE", GLDM_DE },
		{ "GLDM_LGLE", GLDM_LGLE },
		{ "GLDM_HGLE", GLDM_HGLE },
		{ "GLDM_SDLGLE", GLDM_SDLGLE },
		{ "GLDM_SDHGLE", GLDM_SDHGLE },
		{ "GLDM_LDLGLE", GLDM_LDLGLE },
		{ "GLDM_LDHGLE", GLDM_LDHGLE },

		{ "NGTDM_COARSENESS", NGTDM_COARSENESS },
		{ "NGTDM_CONTRAST", NGTDM_CONTRAST },
		{ "NGTDM_BUSYNESS", NGTDM_BUSYNESS },
		{ "NGTDM_COMPLEXITY", NGTDM_COMPLEXITY },
		{ "NGTDM_STRENGTH", NGTDM_STRENGTH },

		{"ZERNIKE2D", ZERNIKE2D},
		{"FRAC_AT_D", FRAC_AT_D },
		{"MEAN_FRAC", MEAN_FRAC },
		{"RADIAL_CV", RADIAL_CV },

		{ "SPAT_MOMENT_00", SPAT_MOMENT_00},
		{ "SPAT_MOMENT_01", SPAT_MOMENT_01},
		{ "SPAT_MOMENT_02", SPAT_MOMENT_02},
		{ "SPAT_MOMENT_03", SPAT_MOMENT_03},
		{ "SPAT_MOMENT_10", SPAT_MOMENT_10},
		{ "SPAT_MOMENT_11", SPAT_MOMENT_11},
		{ "SPAT_MOMENT_12", SPAT_MOMENT_12},
		{ "SPAT_MOMENT_20", SPAT_MOMENT_20},
		{ "SPAT_MOMENT_21", SPAT_MOMENT_21},
		{ "SPAT_MOMENT_30", SPAT_MOMENT_30},

		{ "WEIGHTED_SPAT_MOMENT_00", WEIGHTED_SPAT_MOMENT_00},
		{ "WEIGHTED_SPAT_MOMENT_01", WEIGHTED_SPAT_MOMENT_01},
		{ "WEIGHTED_SPAT_MOMENT_02", WEIGHTED_SPAT_MOMENT_02},
		{ "WEIGHTED_SPAT_MOMENT_03", WEIGHTED_SPAT_MOMENT_03},
		{ "WEIGHTED_SPAT_MOMENT_10", WEIGHTED_SPAT_MOMENT_10},
		{ "WEIGHTED_SPAT_MOMENT_11", WEIGHTED_SPAT_MOMENT_11},
		{ "WEIGHTED_SPAT_MOMENT_12", WEIGHTED_SPAT_MOMENT_12},
		{ "WEIGHTED_SPAT_MOMENT_20", WEIGHTED_SPAT_MOMENT_20},
		{ "WEIGHTED_SPAT_MOMENT_21", WEIGHTED_SPAT_MOMENT_21},
		{ "WEIGHTED_SPAT_MOMENT_30", WEIGHTED_SPAT_MOMENT_30},

		{ "CENTRAL_MOMENT_02", CENTRAL_MOMENT_02},
		{ "CENTRAL_MOMENT_03", CENTRAL_MOMENT_03},
		{ "CENTRAL_MOMENT_11", CENTRAL_MOMENT_11},
		{ "CENTRAL_MOMENT_12", CENTRAL_MOMENT_12},
		{ "CENTRAL_MOMENT_20", CENTRAL_MOMENT_20},
		{ "CENTRAL_MOMENT_21", CENTRAL_MOMENT_21},
		{ "CENTRAL_MOMENT_30", CENTRAL_MOMENT_30},

		{ "WEIGHTED_CENTRAL_MOMENT_02", WEIGHTED_CENTRAL_MOMENT_02 },
		{ "WEIGHTED_CENTRAL_MOMENT_03", WEIGHTED_CENTRAL_MOMENT_03 },
		{ "WEIGHTED_CENTRAL_MOMENT_11", WEIGHTED_CENTRAL_MOMENT_11 },
		{ "WEIGHTED_CENTRAL_MOMENT_12", WEIGHTED_CENTRAL_MOMENT_12 },
		{ "WEIGHTED_CENTRAL_MOMENT_20", WEIGHTED_CENTRAL_MOMENT_20 },
		{ "WEIGHTED_CENTRAL_MOMENT_21", WEIGHTED_CENTRAL_MOMENT_21 },
		{ "WEIGHTED_CENTRAL_MOMENT_30", WEIGHTED_CENTRAL_MOMENT_30 },

		{ "NORM_CENTRAL_MOMENT_02", NORM_CENTRAL_MOMENT_02},
		{ "NORM_CENTRAL_MOMENT_03", NORM_CENTRAL_MOMENT_03},
		{ "NORM_CENTRAL_MOMENT_11", NORM_CENTRAL_MOMENT_11},
		{ "NORM_CENTRAL_MOMENT_12", NORM_CENTRAL_MOMENT_12},
		{ "NORM_CENTRAL_MOMENT_20", NORM_CENTRAL_MOMENT_20},
		{ "NORM_CENTRAL_MOMENT_21", NORM_CENTRAL_MOMENT_21},
		{ "NORM_CENTRAL_MOMENT_30", NORM_CENTRAL_MOMENT_30},

		{ "NORM_SPAT_MOMENT_00", NORM_SPAT_MOMENT_00 },
		{ "NORM_SPAT_MOMENT_01", NORM_SPAT_MOMENT_01 },
		{ "NORM_SPAT_MOMENT_02", NORM_SPAT_MOMENT_02 },
		{ "NORM_SPAT_MOMENT_03", NORM_SPAT_MOMENT_03 },
		{ "NORM_SPAT_MOMENT_10", NORM_SPAT_MOMENT_10 },
		{ "NORM_SPAT_MOMENT_20", NORM_SPAT_MOMENT_20 },
		{ "NORM_SPAT_MOMENT_30", NORM_SPAT_MOMENT_30 },

		{ "HU_M1", HU_M1 },
		{ "HU_M2", HU_M2 },
		{ "HU_M3", HU_M3},
		{ "HU_M4", HU_M4},
		{ "HU_M5", HU_M5},
		{ "HU_M6", HU_M6},
		{ "HU_M7", HU_M7},

		{ "WEIGHTED_HU_M1", WEIGHTED_HU_M1},
		{ "WEIGHTED_HU_M2", WEIGHTED_HU_M2},
		{ "WEIGHTED_HU_M3", WEIGHTED_HU_M3},
		{ "WEIGHTED_HU_M4", WEIGHTED_HU_M4},
		{ "WEIGHTED_HU_M5", WEIGHTED_HU_M5},
		{ "WEIGHTED_HU_M6", WEIGHTED_HU_M6},
		{ "WEIGHTED_HU_M7", WEIGHTED_HU_M7},

		{ "GABOR", GABOR },
	};
} // namespace Nyxus

FeatureSet::FeatureSet()
{
	enableAll(true);
}

bool FeatureSet::findFeatureByString(const std::string& featureName, AvailableFeatures& f)
{
	if (Nyxus::UserFacingFeatureNames.find(featureName) == Nyxus::UserFacingFeatureNames.end())
		return false;

	f = Nyxus::UserFacingFeatureNames[featureName];
	return true;
}

std::string FeatureSet::findFeatureNameByCode(AvailableFeatures fcode)
{
	// Search
	auto result = std::find_if(
		Nyxus::UserFacingFeatureNames.begin(),
		Nyxus::UserFacingFeatureNames.end(),
		[fcode](const auto& finfo) 
		{ return finfo.second == fcode; });

	// Return the feature name if found
	if (result != Nyxus::UserFacingFeatureNames.end())
		return result->first;

	// Nothing is found
	return "[UNNAMED FEATURE]";
}

// Relying on RVO rather than std::move
std::vector<std::tuple<std::string, AvailableFeatures>> FeatureSet::getEnabledFeatures()
{
	std::vector<std::tuple<std::string, AvailableFeatures>> F;
	for (int i = 0; i < AvailableFeatures::_COUNT_; i++)
	{
		if (m_enabledFeatures[i])
		{
			// Find feature i's name
			std::string fname = "feature";
			for (const auto& f : Nyxus::UserFacingFeatureNames)
			{
				if (f.second == i)
				{
					fname = f.first;
					break;
				}
			}

			// Save the pair
			std::tuple<std::string, AvailableFeatures> f(fname, (AvailableFeatures)i); 
			F.push_back(f);
		}
	}
	return F;
}

