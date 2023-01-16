#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>
#include <exception>
#include <iterator>
#include "environment.h"
#include "featureset.h"
#include "features/glcm.h"
#include "helpers/helpers.h"
#include "helpers/system_resource.h"
#include "version.h"

namespace Nyxus
{
	bool existsOnFilesystem(const std::string &);

	bool parse_as_float(const std::string& raw, float &result)
	{
		if (sscanf(raw.c_str(), "%f", &result) != 1)
			return false;
		else
			return true;
	}

	bool parse_as_int(const std::string& raw, int& result)
	{
		if (sscanf(raw.c_str(), "%d", &result) != 1)
			return false;
		else
			return true;
	}

	bool parse_delimited_string_list_to_ints (const std::string & rawString, std::vector<int> & result)
	{
		// It's legal to not have rotation angles specified
		if (rawString.length() == 0)
			return true;

		bool retval = true;
		std::vector<std::string> strings;
		parse_delimited_string(rawString, ",", strings);
		result.clear();
		for (auto &s : strings)
		{
			int v;
			if (!parse_as_int(s, v))
			{
				retval = false;
				std::cout << "Error: in '" << rawString << "' expecting '" << s << "' to be an integer number\n";
			}
			else
				result.push_back(v);
		}
		return retval;
	}

	std::string toupper(const std::string &s)
	{
		auto s_uppr = s;
		for (auto &c : s_uppr)
			c = ::toupper(c);
		return s_uppr;
	}

	bool parse_delimited_string_list_to_features(const std::string &rawString, std::vector<std::string> &result)
	{
		result.clear();

		if (rawString.length() == 0)
		{
			std::cout <<  "Warning: no features specified, defaulting to " << FEA_NICK_ALL << "\n";
			result.push_back(FEA_NICK_ALL);
			return true;
		}

		bool retval = true;
		std::vector<std::string> strings;
		parse_delimited_string(rawString, ",", strings);

		// Check individual features
		for (const auto &s : strings)
		{
			// Forgive user's typos of consequtive commas e.g. MIN,MAX,,MEDIAN
			if (s.empty())
				continue;

			auto s_uppr = toupper(s);
			if (s_uppr == FEA_NICK_ALL ||
				s_uppr == FEA_NICK_ALL_INTENSITY ||
				s_uppr == FEA_NICK_ALL_MORPHOLOGY ||
				s_uppr == FEA_NICK_BASIC_MORPHOLOGY ||
				s_uppr == FEA_NICK_ALL_GLCM ||
				s_uppr == FEA_NICK_ALL_GLRLM ||
				s_uppr == FEA_NICK_ALL_GLSZM ||
				s_uppr == FEA_NICK_ALL_GLDM ||
				s_uppr == FEA_NICK_ALL_NGTDM ||
				s_uppr == FEA_NICK_ALL_BUT_GABOR ||
				s_uppr == FEA_NICK_ALL_BUT_GLCM || 
				s_uppr == FEA_NICK_ALL_EASY ||
				s_uppr == FEA_NICK_ALL_NEIG)
			{
				result.push_back(s_uppr);
				continue;
			}

			AvailableFeatures af;
			bool fnameExists = theFeatureSet.findFeatureByString(s_uppr, af);
			if (!fnameExists)
			{
				retval = false;
				std::cout << "Error: expecting '" << s << "' to be a proper feature name or feature file path\n";
			}
			else
				result.push_back(s_uppr);
		}

		// Show help on available features if necessary
		if (!retval)
		   theEnvironment.show_featureset_help();

		return retval;
	}

	std::string strip_punctn_and_comment (const std::string& src)
	{
		std::string s = src;
		bool commentState = false;
		for (size_t i = 0; i < s.size(); i++)
		{
			if (s[i] == '#')
			{
				commentState = true;
				s.erase(i, 1); // remove ith char from string
				i--; // reduce i with one so you don't miss any char
				continue;
			}

			if (commentState)
			{
				s.erase(i, 1); // remove ith char from string
				i--; // reduce i with one so you don't miss any char
				continue;
			}

			if (s[i] == '\n')
			{
				commentState = false;
				s.erase(i, 1); // remove ith char from string
				i--; // reduce i with one so you don't miss any char
				continue;
			}

			if (!((std::isalnum(s[i]) || s[i] == '_') || s[i] == ','))
			{
				s.erase(i, 1); // remove ith char from string
				i--; // reduce i with one so you don't miss any char
				continue;
			}
		}

		return s;
	}
}

Environment::Environment(): BasicEnvironment()
{
	unsigned long long availMem = Nyxus::getAvailPhysMemory();
	ramLimit = availMem / 2;	// Safely require 50% of available memory
}

size_t Environment::get_ram_limit()
{
	return ramLimit;
}

int Environment::get_pixel_distance()
{
	return n_pixel_distance;
}

void Environment::set_pixel_distance(int pixelDistance)
{
	this->n_pixel_distance = pixelDistance;
}

void Environment::show_cmdline_help()
{
	std::cout
		<< PROJECT_NAME << " " << PROJECT_VER << "\nCopyright Axle Informatics 2021-2022\n"
		<< "Usage:\t" << "nyxus" 
		<< "\t" << FILEPATTERN << " <file pattern regular expression e.g. .*, *.tif, etc (default = .*)> \n"
		<< "\t\t" << OUTPUTTYPE << " <'separatecsv'[default] or 'singlecsv'> \n"
		<< "\t\t" << SEGDIR << " <directory of segmentation images> \n"
		<< "\t\t" << INTDIR << " <directory of intensity images> \n"
		<< "\t\t" << OUTDIR << " <output directory> \n"
		<< "\t\t[ " << FEATURES << " <specific feature or '*all*' (default = '*all*')> ] \n"
		<< "\t\t[ " << XYRESOLUTION << " <number of pixels per centimeter, an integer or floating point number> ] \n"
		<< "\t\t[ " << EMBPIXSZ << " <[default = 0]> ]\n"
		<< "\t\t[ " << LOADERTHREADS << " <number of image loader threads [default = 1]> ] \n"
		<< "\t\t[ " << PXLSCANTHREADS << " <number of pixel scanner threads within a TIFF tile (default = 1)> ] \n"
		<< "\t\t[ " << REDUCETHREADS << " <number of feature reduction threads [default = 1]> ] \n"
		<< "\t\t[ " << PXLDIST << " <number of pixels as neighbor features radius [default = 5]> ] \n"
		<< "\t\t[ " << COARSEGRAYDEPTH << " <custom number of grayscale levels (default: 256)> ] \n"
		<< "\t\t[ " << GLCMANGLES << " <one or more comma separated rotation angles from set {0, 45, 90, and 135}, default is " << GLCMANGLES << "0,45,90,135> ] \n"
		<< "\t\t[ " << VERBOSITY << " <levels of verbosity 0 (silence), 2 (timing), 4 (roi diagnostics), 8 (granular diagnostics) [default = 0]> ] \n"
		<< "\t\t[ " << RAMLIMIT << " <megabytes> ] \n"
		<< "\t\t[ " << TEMPDIR << " <slash-terminating path> ] \n"
		<< "\n"
		<< "\tnyxus -h\tDisplay help info\n"
		<< "\tnyxus --help\tDisplay help info\n";

	#ifdef USE_GPU
		std::cout << " [" << USEGPU << "=<true or false>" << " [" << GPUDEVICEID << "=<valid GPU device ID>] ]\n";
	#endif
}

void Environment::show_summary(const std::string &head, const std::string &tail)
{
	std::cout << head;
	std::cout << "Using " << get_ram_limit() << " bytes of memory\n\n";
	std::cout << "Work plan:\n"
			  << "\tlabels\t" << labels_dir << "\n"
			  << "\tintensities\t" << intensity_dir << "\n"
			  << "\tintensities-to-segmentation map directory\t" << intSegMapDir << "\n"
			  << "\tintensities-to-segmentation map file\t" << intSegMapFile << "\n"
			  << "\toutput\t" << output_dir << "\n"
			  << "\tfile pattern\t" << file_pattern << "\n"
			  << "\tembedded pixel size\t" << embedded_pixel_size << "\n"
			  << "\toutput type\t" << rawOutpType << "\n"
			  << "\t# of image loader threads\t" << n_loader_threads << "\n"
			  << "\t# of pixel scanner threads\t" << n_pixel_scan_threads << "\n"
			  << "\t# of post-processing threads\t" << n_reduce_threads << "\n"
			  << "\tpixel distance\t" << n_pixel_distance << "\n"
			  << "\tverbosity level\t" << verbosity_level << "\n";

#ifdef USE_GPU
	std::cout << "\tusing GPU\t" << (using_gpu() ? "yes" : "no") << "\n";
	if (using_gpu())
		std::cout << "\tGPU device ID \t" << get_gpu_device_choice() << "\n";
#endif

	// Features
	std::cout << "\tfeatures\t";
	for (auto f : recognizedFeatureNames)
	{
		if (f != recognizedFeatureNames[0])
			std::cout << ", ";
		std::cout << f;
	}
	std::cout << "\n";

	// Resolution
	if (xyRes > 0.0)
		std::cout << "\tXY-resolution " << xyRes << "\n";

	// GLCM angles
	std::cout << "\tGLCM angles\t";
	for (auto ang : GLCMFeature::angles)
	{
		if (ang != GLCMFeature::angles[0])
			std::cout << ", ";
		std::cout << ang;
	}
	std::cout << "\n";

	// Oversized ROI limit
	std::cout << "\tbatch and oversized ROI lower limit " << theEnvironment.get_ram_limit() << " bytes\n";

	// Temp directory
	std::cout << "\ttemp directory " << theEnvironment.get_temp_dir_path() << "\n";

	std::cout << tail;
}

bool Environment::find_string_argument(std::vector<std::string>::iterator &i, const char *arg, std::string &arg_value)
{
	std::string actualArgName = *i;

	// Syntax #1 <arg> <value>
	std::string a = arg;
	if (actualArgName == a)
	{
		arg_value = *++i;
		recognizedArgs.push_back({a, arg_value});
		return true;
	}
	else
	{
		// Syntax #2 <arg>=<value>
		a.append("=");
		auto pos = actualArgName.find(a);
		if (pos != std::string::npos)
		{
			arg_value = actualArgName.substr(a.length());
			recognizedArgs.push_back({a, arg_value});
			return true;
		}
	}

	// Argument was not recognized
	return false;
}

bool Environment::find_int_argument(std::vector<std::string>::iterator &i, const char *arg, int &arg_value)
{
	// Syntax #1
	std::string a = arg;
	if (*i == a)
	{
		std::string val = *++i;
		// string -> integer
		if (sscanf(val.c_str(), "%d", &arg_value) != 1)
			return true;
	}
	else
	{
		// Syntax #2
		a.append("=");
		auto pos = (*i).find(a);
		if (pos != std::string::npos)
		{
			std::string val = (*i).substr(a.length());
			// string -> integer
			if (sscanf(val.c_str(), "%d", &arg_value) != 1)
				return true;
		}
	}

	// Argument was not recognized
	return false;
}

void Environment::process_feature_list()
{
	theFeatureSet.enableAll(false); // First, disable all
	for (auto &s : recognizedFeatureNames) // Second, iterate uppercased feature names
	{
		// Check if features are requested via a group nickname
		if (s == FEA_NICK_ALL)
		{
			theFeatureSet.enableAll();
			break; // No need to bother of others
		}
		if (s == FEA_NICK_ALL_BUT_GABOR)
		{
			theFeatureSet.enableAll();
			auto F = {GABOR};
			theFeatureSet.disableFeatures(F);
			break; // No need to bother of others
		}
		if (s == FEA_NICK_ALL_BUT_GLCM)
		{
			theFeatureSet.enableAll();
			auto F = {
				GLCM_ANGULAR2NDMOMENT,
				GLCM_CONTRAST,
				GLCM_CORRELATION,
				GLCM_VARIANCE,
				GLCM_INVERSEDIFFERENCEMOMENT,
				GLCM_SUMAVERAGE,
				GLCM_SUMVARIANCE,
				GLCM_SUMENTROPY,
				GLCM_ENTROPY,
				GLCM_DIFFERENCEVARIANCE,
				GLCM_DIFFERENCEENTROPY,
				GLCM_INFOMEAS1,
				GLCM_INFOMEAS2 };
			theFeatureSet.disableFeatures(F);
			break; // No need to bother of others
		}

		if (s == FEA_NICK_ALL_INTENSITY)
		{
			auto F = {
				INTEGRATED_INTENSITY,
				MEAN,
				MEDIAN,
				MIN,
				MAX,
				RANGE,
				STANDARD_DEVIATION,
				STANDARD_ERROR,
				UNIFORMITY,
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
				P01, P10, P25, P75, P90, P99,
				INTERQUARTILE_RANGE,
				ROBUST_MEAN_ABSOLUTE_DEVIATION };
			theFeatureSet.enableFeatures(F);
			continue;
		}
		if (s == FEA_NICK_ALL_MORPHOLOGY)
		{
			auto F = {
				AREA_PIXELS_COUNT,
				AREA_UM2,
				CENTROID_X,
				CENTROID_Y,
				DIAMETER_EQUAL_AREA, 
				WEIGHTED_CENTROID_Y,
				WEIGHTED_CENTROID_X,
				COMPACTNESS,
				BBOX_YMIN,
				BBOX_XMIN,
				BBOX_HEIGHT,
				BBOX_WIDTH,
				MAJOR_AXIS_LENGTH,
				MINOR_AXIS_LENGTH,
				ECCENTRICITY,
				ORIENTATION,
				ROUNDNESS,
				EXTENT,
				ASPECT_RATIO,
				DIAMETER_EQUAL_PERIMETER,
				CONVEX_HULL_AREA,
				SOLIDITY,
				PERIMETER,
				EDGE_MEAN_INTENSITY,
				EDGE_STDDEV_INTENSITY,
				EDGE_MAX_INTENSITY,
				EDGE_MIN_INTENSITY,
				CIRCULARITY, 
				MASS_DISPLACEMENT };
			theFeatureSet.enableFeatures(F);
			continue;
		}
		if (s == FEA_NICK_BASIC_MORPHOLOGY)
		{
			auto F = {
				AREA_PIXELS_COUNT,
				AREA_UM2,
				CENTROID_X,
				CENTROID_Y,
				BBOX_YMIN,
				BBOX_XMIN,
				BBOX_HEIGHT,
				BBOX_WIDTH };
			theFeatureSet.enableFeatures(F);
			continue;
		}
		if (s == FEA_NICK_ALL_GLCM)
		{
			auto F = {
				GLCM_ANGULAR2NDMOMENT,
				GLCM_CONTRAST,
				GLCM_CORRELATION,
				GLCM_DIFFERENCEAVERAGE,
				GLCM_DIFFERENCEENTROPY,
				GLCM_DIFFERENCEVARIANCE,
				GLCM_ENERGY,
				GLCM_ENTROPY,
				GLCM_HOMOGENEITY,
				GLCM_INFOMEAS1,
				GLCM_INFOMEAS2,
				GLCM_INVERSEDIFFERENCEMOMENT,
				GLCM_SUMAVERAGE,
				GLCM_SUMENTROPY,
				GLCM_SUMVARIANCE,
				GLCM_VARIANCE };
			theFeatureSet.enableFeatures(F);
			continue;
		}
		if (s == FEA_NICK_ALL_GLRLM)
		{
			auto F = {
				GLRLM_SRE,
				GLRLM_LRE,
				GLRLM_GLN,
				GLRLM_GLNN,
				GLRLM_RLN,
				GLRLM_RLNN,
				GLRLM_RP,
				GLRLM_GLV,
				GLRLM_RV,
				GLRLM_RE,
				GLRLM_LGLRE,
				GLRLM_HGLRE,
				GLRLM_SRLGLE,
				GLRLM_SRHGLE,
				GLRLM_LRLGLE,
				GLRLM_LRHGLE };
			theFeatureSet.enableFeatures(F);
			continue;
		}
		if (s == FEA_NICK_ALL_GLSZM)
		{
			auto F = {
				GLSZM_SAE,
				GLSZM_LAE,
				GLSZM_GLN,
				GLSZM_GLNN,
				GLSZM_SZN,
				GLSZM_SZNN,
				GLSZM_ZP,
				GLSZM_GLV,
				GLSZM_ZV,
				GLSZM_ZE,
				GLSZM_LGLZE,
				GLSZM_HGLZE,
				GLSZM_SALGLE,
				GLSZM_SAHGLE,
				GLSZM_LALGLE,
				GLSZM_LAHGLE };
			theFeatureSet.enableFeatures(F);
			continue;
		}
		if (s == FEA_NICK_ALL_GLDM)
		{
			auto F = {
				GLDM_SDE,
				GLDM_LDE,
				GLDM_GLN,
				GLDM_DN,
				GLDM_DNN,
				GLDM_GLV,
				GLDM_DV,
				GLDM_DE,
				GLDM_LGLE,
				GLDM_HGLE,
				GLDM_SDLGLE,
				GLDM_SDHGLE,
				GLDM_LDLGLE,
				GLDM_LDHGLE };
			theFeatureSet.enableFeatures(F);
			continue;
		}
		if (s == FEA_NICK_ALL_NGTDM)
		{
			auto F = {
				NGTDM_COARSENESS,
				NGTDM_CONTRAST,
				NGTDM_BUSYNESS,
				NGTDM_COMPLEXITY,
				NGTDM_STRENGTH };
			theFeatureSet.enableFeatures(F);
			continue;
		}

		if (s == FEA_NICK_ALL_EASY)
		{
			theFeatureSet.enableAll();
			auto F = {
				//=== Gabor
				GABOR,

				//=== GLCM
				GLCM_ANGULAR2NDMOMENT,
				GLCM_CONTRAST,
				GLCM_CORRELATION,
				GLCM_VARIANCE,
				GLCM_INVERSEDIFFERENCEMOMENT,
				GLCM_SUMAVERAGE,
				GLCM_SUMVARIANCE,
				GLCM_SUMENTROPY,
				GLCM_ENTROPY,
				GLCM_DIFFERENCEVARIANCE,
				GLCM_DIFFERENCEENTROPY,
				GLCM_INFOMEAS1,
				GLCM_INFOMEAS2,

				//=== 2D moments

				// Spatial (raw) moments
				SPAT_MOMENT_00,
				SPAT_MOMENT_01,
				SPAT_MOMENT_02,
				SPAT_MOMENT_03,
				SPAT_MOMENT_10,
				SPAT_MOMENT_11,
				SPAT_MOMENT_12,
				SPAT_MOMENT_20,
				SPAT_MOMENT_21,
				SPAT_MOMENT_30,

				// Weighted spatial moments
				WEIGHTED_SPAT_MOMENT_00,
				WEIGHTED_SPAT_MOMENT_01,
				WEIGHTED_SPAT_MOMENT_02,
				WEIGHTED_SPAT_MOMENT_03,
				WEIGHTED_SPAT_MOMENT_10,
				WEIGHTED_SPAT_MOMENT_11,
				WEIGHTED_SPAT_MOMENT_12,
				WEIGHTED_SPAT_MOMENT_20,
				WEIGHTED_SPAT_MOMENT_21,
				WEIGHTED_SPAT_MOMENT_30,

				// Central moments
				CENTRAL_MOMENT_02,
				CENTRAL_MOMENT_03,
				CENTRAL_MOMENT_11,
				CENTRAL_MOMENT_12,
				CENTRAL_MOMENT_20,
				CENTRAL_MOMENT_21,
				CENTRAL_MOMENT_30,

				// Weighted central moments
				WEIGHTED_CENTRAL_MOMENT_02,
				WEIGHTED_CENTRAL_MOMENT_03,
				WEIGHTED_CENTRAL_MOMENT_11,
				WEIGHTED_CENTRAL_MOMENT_12,
				WEIGHTED_CENTRAL_MOMENT_20,
				WEIGHTED_CENTRAL_MOMENT_21,
				WEIGHTED_CENTRAL_MOMENT_30,

				// Normalized central moments
				NORM_CENTRAL_MOMENT_02,
				NORM_CENTRAL_MOMENT_03,
				NORM_CENTRAL_MOMENT_11,
				NORM_CENTRAL_MOMENT_12,
				NORM_CENTRAL_MOMENT_20,
				NORM_CENTRAL_MOMENT_21,
				NORM_CENTRAL_MOMENT_30,

				// Normalized (standardized) spatial moments
				NORM_SPAT_MOMENT_00,
				NORM_SPAT_MOMENT_01,
				NORM_SPAT_MOMENT_02,
				NORM_SPAT_MOMENT_03,
				NORM_SPAT_MOMENT_10,
				NORM_SPAT_MOMENT_20,
				NORM_SPAT_MOMENT_30,

				// Hu's moments 1-7 
				HU_M1,
				HU_M2,
				HU_M3,
				HU_M4,
				HU_M5,
				HU_M6,
				HU_M7,

				// Weighted Hu's moments 1-7 
				WEIGHTED_HU_M1,
				WEIGHTED_HU_M2,
				WEIGHTED_HU_M3,
				WEIGHTED_HU_M4,
				WEIGHTED_HU_M5,
				WEIGHTED_HU_M6,
				WEIGHTED_HU_M7 };

			theFeatureSet.disableFeatures(F);

			break; // No need to bother of others
		}

		if (s == FEA_NICK_ALL_NEIG)
		{
			auto F = {
				NUM_NEIGHBORS,
				PERCENT_TOUCHING,
				CLOSEST_NEIGHBOR1_DIST,
				CLOSEST_NEIGHBOR1_ANG,
				CLOSEST_NEIGHBOR2_DIST,
				CLOSEST_NEIGHBOR2_ANG,
				ANG_BW_NEIGHBORS_MEAN,
				ANG_BW_NEIGHBORS_STDDEV,
				ANG_BW_NEIGHBORS_MODE };
			theFeatureSet.enableFeatures(F);
			break; // No need to bother of others
		}
		// Process features individually
		AvailableFeatures af;
		if (!theFeatureSet.findFeatureByString(s, af))
		{
			throw std::invalid_argument("Error: '" + s + "' is not a valid feature name. \n");
		}

		theFeatureSet.enableFeature(af);
	}
}

int Environment::parse_cmdline(int argc, char **argv)
{
	// Program being run without any flags and options?
	if (argc == 1)
		return 1;

	std::vector<std::string> args(argv + 1, argv + argc);
	std::vector<std::string> unrecognizedArgs;

	//==== Gather raw data
	for (auto i = args.begin(); i != args.end(); ++i)
	{
		if (*i == "-h" || *i == "--help")
		{
			show_cmdline_help();
			return 1;
		}

		if (!(
				find_string_argument(i, INTDIR, intensity_dir) ||
				find_string_argument(i, SEGDIR, labels_dir) ||
				find_string_argument(i, OUTDIR, output_dir) ||
				find_string_argument(i, INTSEGMAPDIR, intSegMapDir) ||
				find_string_argument(i, INTSEGMAPFILE, intSegMapFile) ||
				find_string_argument(i, FEATURES, rawFeatures) ||
				find_string_argument(i, XYRESOLUTION, rawXYRes) ||
				find_string_argument(i, FILEPATTERN, file_pattern) ||
				find_string_argument(i, OUTPUTTYPE, rawOutpType) ||
				find_string_argument(i, EMBPIXSZ, embedded_pixel_size) ||
				find_string_argument(i, LOADERTHREADS, loader_threads) ||
				find_string_argument(i, PXLSCANTHREADS, pixel_scan_threads) ||
				find_string_argument(i, REDUCETHREADS, reduce_threads) ||
				find_string_argument(i, GLCMANGLES, rawGlcmAngles) ||
				find_string_argument(i, PXLDIST, pixel_distance) ||
				find_string_argument(i, COARSEGRAYDEPTH, raw_coarse_grayscale_depth) ||
				find_string_argument(i, VERBOSITY, verbosity) ||
				find_string_argument(i, RAMLIMIT, rawRamLimit) ||
				find_string_argument(i, TEMPDIR, rawTempDir)
#ifdef USE_GPU
				|| find_string_argument(i, USEGPU, rawUseGpu) 
				|| find_string_argument(i, GPUDEVICEID, rawGpuDeviceID) 
#endif
			))
			unrecognizedArgs.push_back(*i);
	}

	//==== Show the user recognized and unrecognized command line elements

	// --include the raw command line
	std::stringstream rawCL;
	rawCL << "\nCommand line:\n" << argv[0] << " ";
	std::copy(args.begin(), args.end(), std::ostream_iterator<std::string>(rawCL, " ")); // vector of strings -> string
	rawCL << "\n\n";

	std::cout << "\nAccepted command line arguments:\n";
	for (auto& m : recognizedArgs)
		std::cout << "\t" << std::get<0>(m) << " : " << std::get<1>(m) << "\n";

	// --what's not recognized?
	if (unrecognizedArgs.size() > 0)
	{
		std::cout << "Ignoring unrecognized arguments:\n";
		for (auto &u : unrecognizedArgs)
			std::cout << "\t" << u << "\n";
	}

	//==== Check mandatory parameters

	if (file_pattern == "")
	{
		std::cout << "Error: Missing argument " << FILEPATTERN << "\n";
		return 1;
	}
	if (check_file_pattern(file_pattern) == false)
	{
		std::cout << "Error: file pattern '" << file_pattern << "' is an invalid regular expression\n";
		return 1;
	}

	if (labels_dir == "")
	{
		std::cout << "Error: Missing argument " << SEGDIR << "\n";
		return 1;
	}
	if (intensity_dir == "")
	{
		std::cout << "Error: Missing argument " << INTDIR << "\n";
		return 1;
	}
	if (output_dir == "")
	{
		std::cout << "Error: Missing argument " << OUTDIR << "\n";
		return 1;
	}

	if (rawOutpType == "")
	{
		std::cout << "Error: Missing argument " << OUTPUTTYPE << "\n";
		return 1;
	}

	if (rawFeatures == "")
	{
		std::cout << "Warning: " << FEATURES << "=<empty string>, defaulting to " << FEA_NICK_ALL << "\n";
		rawFeatures = FEA_NICK_ALL;
	}

	//==== Output type
	auto rawOutpTypeUC = Nyxus::toupper(rawOutpType);
	if (rawOutpTypeUC != Nyxus::toupper(OT_SINGLECSV) && rawOutpTypeUC != Nyxus::toupper(OT_SEPCSV))
	{
		std::cout << "Error: valid values of " << OUTPUTTYPE << " are " << OT_SEPCSV << " or " << OT_SINGLECSV << "\n";
		return 1;
	}
	separateCsv = rawOutpTypeUC == Nyxus::toupper(OT_SEPCSV);

	//==== Check numeric parameters
	if (!loader_threads.empty())
	{
		// string -> integer
		if (sscanf(loader_threads.c_str(), "%d", &n_loader_threads) != 1 || n_loader_threads <= 0)
		{
			std::cout << "Error: " << LOADERTHREADS << "=" << loader_threads << ": expecting a positive integer constant\n";
			return 1;
		}
	}

	if (!pixel_scan_threads.empty())
	{
		// string -> integer
		if (sscanf(pixel_scan_threads.c_str(), "%d", &n_pixel_scan_threads) != 1 || n_pixel_scan_threads <= 0)
		{
			std::cout << "Error: " << PXLSCANTHREADS << "=" << pixel_scan_threads << ": expecting a positive integer constant\n";
			return 1;
		}
	}

	if (!reduce_threads.empty())
	{
		// string -> integer
		if (sscanf(reduce_threads.c_str(), "%d", &n_reduce_threads) != 1 || n_reduce_threads <= 0)
		{
			std::cout << "Error: " << REDUCETHREADS << "=" << reduce_threads << ": expecting a positive integer constant\n";
			return 1;
		}
	}

	if (!pixel_distance.empty())
	{
		// string -> integer
		if (sscanf(pixel_distance.c_str(), "%d", &n_pixel_distance) != 1 || n_pixel_distance <= 0)
		{
			std::cout << "Error: " << PXLDIST << "=" << pixel_distance << ": expecting a positive integer constant\n";
			return 1;
		}
	}

	// parse COARSEGRAYDEPTH
	if (!raw_coarse_grayscale_depth.empty())
	{
		// string -> integer
		if (sscanf(raw_coarse_grayscale_depth.c_str(), "%d", &coarse_grayscale_depth) != 1 || coarse_grayscale_depth <= 0)
		{
			std::cout << "Error: " << COARSEGRAYDEPTH << "=" << raw_coarse_grayscale_depth << ": expecting a positive integer constant\n";
			return 1;
		}
	}

	if (!verbosity.empty())
	{
		// string -> integer
		if (sscanf(verbosity.c_str(), "%d", &verbosity_level) != 1 || verbosity_level < 0)
		{
			std::cout << "Error: " << VERBOSITY << "=" << reduce_threads << ": expecting a positive integer constant\n";
			return 1;
		}
	}

	//==== Parse rotations
	if (!rawGlcmAngles.empty())
	{
		if (!Nyxus::parse_delimited_string_list_to_ints (rawGlcmAngles, glcmAngles))
		{
			std::cout << "Error parsing a list of integers " << rawGlcmAngles << "\n";
			return 1;
		}

		// The angle list parsed well, let's tell it to GLCMFeature 
		GLCMFeature::angles = glcmAngles;
	}

	//==== Parse the RAM limit
	if (!rawRamLimit.empty())
	{
		// string -> integer
		size_t value = 0;
		auto scanfResult = sscanf(rawRamLimit.c_str(), "%zu", &value);
		if (scanfResult != 1 || value < 0)
		{
			std::cout << "Error: " << RAMLIMIT << "=" << rawRamLimit << ": expecting a non-negative integer constant (RAM limit in megabytes)\n";
			return 1;
		}

		// To megabytes
		value *= 1048576;

		// Check if it over the actual limit
		unsigned long long actualRam = Nyxus::getAvailPhysMemory();
		if (value > actualRam)
		{
			std::cout << "Error: RAM limit " << value << " is over the actual amount of available RAM " << actualRam << "\n";
			return 1;
		}

		// The angle list parsed well, let's tell it to GLCMFeature 
		ramLimit = value;
	}

	//==== Parse the temp directory
	if (!rawTempDir.empty())
	{
		// Check the path
		if (!existsOnFilesystem(rawTempDir))
		{
			std::cout << "Error :" << TEMPDIR << "=" << rawTempDir << ": nonexisting directory\n";
			return 1;
		}
		
		// Modify the temp directory path
		this->temp_dir_path = rawTempDir + "\\";
	}

	//==== Using GPU
	#ifdef USE_GPU
	auto rawUseGpuUC = Nyxus::toupper(rawUseGpu);
	if (rawUseGpuUC.length() == 0)
	{
		use_gpu_ = false;
		std::cout << "\n!\n! Not using GPU. To involve GPU, use command line option " << USEGPU << "=true\n!\n\n";
	}
	else
	{
		auto validUsegpu1 = Nyxus::toupper("true"), 
			validUsegpu2 = Nyxus::toupper("false");
		if (rawUseGpuUC != validUsegpu1 && rawUseGpuUC != validUsegpu2)
		{
			std::cerr << "Error: valid values of " << USEGPU << " are " << validUsegpu1 << " or " << validUsegpu2 << "\n";
			return 1;
		}
		use_gpu_ = rawUseGpuUC == validUsegpu1;

		// Process user's GPU device choice
		if (use_gpu_)
		{
			if (!rawGpuDeviceID.empty())
			{
				// string -> integer
				if (sscanf(rawGpuDeviceID.c_str(), "%d", &gpu_device_id_) != 1 || gpu_device_id_ < 0)
				{
					std::cout << "Error: " << GPUDEVICEID << "=" << gpu_device_id_ << ": expecting 0 or positive integer constant\n";
					return 1;
				}
			}
			else
				gpu_device_id_ = 0;	// Specific GPU device ID was not requested, defaulting to 0
		}
	}
	#endif

	//==== Parse desired features

	// --Try to pick up features from a file
	if (rawFeatures.length() > 0 && Nyxus::existsOnFilesystem(rawFeatures))
	{
		std::ifstream file(rawFeatures);
		std::string ln, featureList;
		while (std::getline(file, ln))
		{
			// Strip punctuation and comment text
			std::string pureLn = strip_punctn_and_comment(ln);

			// Skip empty strings
			if (pureLn.length() == 0)
				continue;

			// Consume the purified feature name
			// --insert comma after 1st item
			if (featureList.length() > 0)
				// --no need for inserted comma if the item came with its own comma
				if (pureLn[pureLn.size()-1] != ',')
					featureList += ",";
			featureList += pureLn;
		}

		std::cout << "Using features [" << featureList << "] from file " << rawFeatures << "\n";

		// Modify the input string
		rawFeatures = featureList;
	}

	// --Make sure all the feature names are legal and cast to uppercase (class FeatureSet understands uppercase names)
	if (!Nyxus::parse_delimited_string_list_to_features(rawFeatures, recognizedFeatureNames))
	{
		std::cerr << "Stopping due to errors while parsing user requested features\n";
		return 1;
	}

	// --Feature names are ok, set the flags
	try
	{
		process_feature_list();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what();
		return 1;
	}

	// --Handle the whole-slide mode
	if (Nyxus::toupper(labels_dir) == Nyxus::toupper(intensity_dir))
	{
		singleROI = true;
		std::cout << 
			"+-----------------------------------------------------------+\n"
			"|                                                           |\n"
			"|  Activating whole slide (aka single-ROI) mode             |\n"
			"|  ATTENTION: disabling time-sonsuming erosions features    |\n"
			"|                                                           |\n"
			"+-----------------------------------------------------------+\n" ;
		
		auto F = {
			EROSIONS_2_VANISH,
			EROSIONS_2_VANISH_COMPLEMENT, 
			GABOR
		};
		theFeatureSet.disableFeatures (F);
	}

	//==== Parse resolution
	if (rawXYRes.length() > 0)
	{
		// string -> number
		if (sscanf(rawXYRes.c_str(), "%f", &xyRes) != 1 || xyRes <= 0)
		{
			std::cout << "Error: " << XYRESOLUTION << "=" << xyRes << ": expecting a positive numeric constant\n";
			return 1;
		}
		// pixel size
		pixelSizeUm = 1e-2f / xyRes / 1e-6f; // 1 cm in meters / pixels per cm / micrometers
	}

	// Success
	return 0;
}

void Environment::show_featureset_help()
{
	const int W = 40;   // width

	std::cout << "\n" << 
		"Available features : " << "\n" << 
		"-------------------- " <<
		"\n";
	for (auto f = Nyxus::UserFacingFeatureNames.begin(); f != Nyxus::UserFacingFeatureNames.end(); ++f) // (const auto& f : Nyxus::UserFacingFeatureNames)
	{
		auto idx = std::distance (Nyxus::UserFacingFeatureNames.begin(), f);

		std::cout << std::setw(W) << f->first << " ";
		if ((idx + 1) % 4 == 0)
			std::cout << "\n";
	}
	std::cout << "\n";

	std::vector<std::string> fgroups =
	{
		FEA_NICK_ALL,
		FEA_NICK_ALL_EASY,
		FEA_NICK_ALL_INTENSITY,
		FEA_NICK_ALL_MORPHOLOGY,
		FEA_NICK_BASIC_MORPHOLOGY,
		FEA_NICK_ALL_GLCM,
		FEA_NICK_ALL_GLRLM,
		FEA_NICK_ALL_GLSZM,
		FEA_NICK_ALL_GLDM,
		FEA_NICK_ALL_NGTDM,
		FEA_NICK_ALL_BUT_GABOR,
		FEA_NICK_ALL_BUT_GLCM,
		FEA_NICK_ALL_NEIG
	};

	std::cout << "\n" << 
		"Available feature groups :" << "\n" <<
		"--------------------------" << "\n";
	for (const auto& f : fgroups)
		std::cout << std::setw(W) << f << "\n";
	std::cout << "\n";
}

int Environment::get_floating_point_precision()
{
	return floating_point_precision;
}

unsigned int Environment::get_coarse_gray_depth()
{
	return coarse_grayscale_depth;
}

void Environment::set_coarse_gray_depth (unsigned int new_depth)
{
	coarse_grayscale_depth = new_depth;
}

bool Environment::gpu_is_available() { 
	#ifdef USE_GPU
    	return get_gpu_properties().size() > 0 ? true : false;
	#else
		return false;
	#endif
}

#ifdef USE_GPU

void Environment::set_gpu_device_id(int choice){

	int num_gpus = get_gpu_properties().size();

	if(num_gpus == 0) {
		std::cout << "No gpu available." << std::endl;
		return;
	}

	if(choice > get_gpu_properties().size()-1) {
		std::cout << "GPU choice out of range. Defaulting to device 0." << std::endl;
		gpu_device_id_ = 0;
		return;
	}

	gpu_device_id_ = choice;
}

int Environment::get_gpu_device_choice()
{
	if (using_gpu())
		return gpu_device_id_;
	else
		return -1;  // GPU was not requested so return an invalid device ID -1
}

void Environment::set_use_gpu(bool yes)
{
	use_gpu_ = yes;
}

bool Environment::using_gpu()
{
	return use_gpu_;
}

std::vector<std::map<std::string, std::string>> Environment::get_gpu_properties() {
    int n_devices;
    std::vector<std::map<std::string, std::string>> props;

    cudaGetDeviceCount(&n_devices);

    for(int i = 0; i < n_devices; ++i){
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, i);

        std::map<std::string, std::string> temp;

        temp["Device number"] =  std::to_string(i);
        temp["Device name"] = prop.name;
        temp["Memory"] = std::to_string(prop.totalGlobalMem/pow(2,30)) + " GB";
        temp["Capability"] = std::to_string(prop.major) + std::to_string(prop.minor);

        props.push_back(temp);
    }

    return props;
}



#endif


