#pragma once

#include <map>
#include <string>
#include <vector>

#include "arrow_output_stream.h"
#include "environment_basic.h"
#include "cli_anisotropy_options.h"
#include "cli_fpimage_options.h"
#include "cli_gabor_options.h"
#include "cli_glcm_options.h"
#include "cli_nested_roi_options.h"
#include "cli_result_options.h"
#include "roi_blacklist.h"
#include "save_option.h"

#ifdef USE_GPU
	#include <cuda_runtime.h>
	#include "cli_gpu_options.h"
#endif

// Command line arguments
#define SEGDIR "--segDir"						// Environment :: labels_dir
#define INTDIR "--intDir"						// Environment :: intensity_dir
#define OUTDIR "--outDir"						// Environment :: output_dir
#define INTSEGMAPDIR "--intSegMapDir"			// get_int_seg_map_dir()
#define INTSEGMAPFILE "--intSegMapFile"			// get_int_seg_map_file()
#define FEATURES "--features"					// Environment :: features	-- Example: (1) --features=area,kurtosis,neighbors (2) --features=featurefile.txt
#define FILEPATTERN "--filePattern"				// Environment :: file_pattern
#define OUTPUTTYPE "--outputType"				// Environment :: Output type for feature values (speratecsv, singlecsv, arrow, parquet)
#define EMBPIXSZ "--embeddedpixelsize"			// Environment :: embedded_pixel_size
#define REDUCETHREADS "--reduceThreads"			// Environment :: n_reduce_threads
#define VERBOSITY "--verbose"					// Environment :: verbosity_level	-- Example: --verbosity=3
#define ONLINESTATSTHRESH "--onlineStatsThresh" // Environment :: onlineStatsThreshold	-- Example: --onlineStatsThresh=150
#define XYRESOLUTION "--pixelsPerCentimeter"	// pixels per centimeter
#define PXLDIST "--pixelDistance"				// used in neighbor features
#define COARSEGRAYDEPTH "--coarseGrayDepth"		// Environment :: raw_coarse_grayscale_depth
#define RAMLIMIT "--ramLimit"					// Optional. Limit for treating ROIs as non-trivial and for setting the batch size of trivial ROIs. Default - amount of available system RAM
#define TEMPDIR "--tempDir"						// Optional. Used in processing non-trivial features. Default - system temp directory
#define IBSICOMPLIANCE "--ibsi" // skip binning for grey level and grey tone features
#define SKIPROI "--skiproi"		// Optional. Skip ROIs having specified labels. Sybtax: --skiproi <label[,label,label,...]>
#define RESULTFNAME "--resultFname"				// Environment :: nyxus_result_fname
#define CLI_DIM "--dim"							// Environment :: raw_dim

#ifdef CHECKTIMING
	#define EXCLUSIVETIMING "--exclusivetiming"
#endif

#ifdef USE_GPU
	#define USEGPU "--useGpu"					// Environment::rawUseGpu, "true" or "false"
	#define GPUDEVICEID "--gpuDeviceID"		// Environment::rawGpuDeviceID
#endif

// Gabor feature CLI arguments
#define GABOR_FREQS "--gaborfreqs"		// Example: "2,4,8,72". Frequencies should atch thetas: --gaborfreqs=1,2,3,4,5 --gabortheta=30,30,45,90,90
#define GABOR_GAMMA "--gaborgamma"		// Example: "0.1"
#define GABOR_SIG2LAM "--gaborsig2lam"	// Example: "0.8"
#define GABOR_KERSIZE "--gaborkersize"	// Example: "20"
#define GABOR_F0 "--gaborf0"			// Example: "0.1"
#define GABOR_THETA "--gabortheta"		// Example: "60,45,90"
#define GABOR_THRESHOLD "--gaborthold"	// Example: "0.025"

// GLCM feature
#define GLCMANGLES "--glcmAngles"				// Environment :: rotAngles
#define GLCMOFFSET "--glcmOff"					// Environment :: raw_glcm_

// Nested ROI functionality
#define NESTEDROI_CHNL_SIGNATURE "--hsig"		// Channel signature Example: "_c" in "p0_y1_r1_c1.ome.tiff"
#define NESTEDROI_PARENT_CHNL "--hpar"			// Channel number that should be used as a provider of parent segments. Example: --hpar=1
#define NESTEDROI_CHILD_CHNL "--hchi"			// Channel number that should be used as a provider of child segments. Example: --hchi=0
#define NESTEDROI_AGGREGATION_METHOD "--hag"	// How to aggregate features of segments recognized as children of same parent segment. See class NestedRoiOptions for options.

// Floating-point voxel image options (served by class FpImageOptions)
#define FPIMAGE_TARGET_DYNRANGE "--fpimgdr"		// Desired dynamic range of the integer voxel intensities converted from floating-point intensities
#define FPIMAGE_MIN "--fpimgmin"				// Expected voxel min intensity
#define FPIMAGE_MAX "--fpimgmax"				// Expected voxel max intensity

// Anisotropy
#define ANISO_X "--anisox"
#define ANISO_Y "--anisoy"
#define ANISO_Z "--anisoz"

// Result options
#define NOVAL "--noval"						// -> raw_noval
#define TINYVAL "--tinyval"					// -> raw_tiny
#define AGGREGATE "--aggr"				// -> raw_aggregate
#define ANNOTATE "--annot"				// -> raw_annotate
#define ANNOT_SEP "--annotsep"		// -> raw_anno_separator

// Valid values of 'OUTPUTTYPE'
#define OT_SEPCSV "separatecsv"
#define OT_SINGLECSV "singlecsv"
#define OT_ARROWIPC "arrowipc"
#define OT_PARQUET "parquet"

// Verbosity levels (combinable via binary and)
#define VERBOSITY_TIMING 2
#define VERBOSITY_ROI_INFO 4
#define VERBOSITY_DETAILED 8


/// @brief Class encapsulating the the feature extraction environment - command line option values, default values, etc. Use it to add a parseable command line parameter.
class Environment: public BasicEnvironment
{
public:
	Environment();
	bool parse_cmdline(int argc, char **argv);
	void show_cmdline_help();
	void show_featureset_help(); 
	void show_summary(const std::string &head, const std::string &tail);

	std::string labels_dir = "",
		intensity_dir = "",
		output_dir = "",
		intSegMapDir = "",
		intSegMapFile = "";
	std::string nyxus_result_fname = "NyxusFeatures";	// Default file name without extension ".csv", ".arrow", etc

	// Returns the expected dataset dimensionality based on the command line options
	int dim() { return dim_; }
	void set_dim(int d) { dim_ = d; }
	bool is_imq() {return is_imq_;};
	void set_imq(bool is_imq) {is_imq_ = is_imq;}

	bool singleROI = false; // Applies to dim()==2: singleROI is set to 'true' parse_cmdline() if labels_dir==intensity_dir

	Nyxus::ArrowOutputStream  arrow_stream;

	std::string embedded_pixel_size = "";

	std::string rawFeatures;
	std::vector<std::string> recognizedFeatureNames;

	std::string reduce_threads = "";
	int n_reduce_threads = 4;

	std::string pixel_distance = "";
	int n_pixel_distance = 5;

	std::string rawVerbosity = "";	// 'verbosity_level' is inherited from BasicEnvironment

	std::string rawOnlineStatsThresh = "";
	int onlineStatsTreshold = 0;

	std::string rawOutpType = ""; // Valid values: "separatecsv", "singlecsv", "arrow", "parquet"
	bool separateCsv = true;

	Nyxus::SaveOption saveOption;

	// x- and y- resolution in pixels per centimeter
	std::string rawXYRes = "";
	float xyRes = 0.0,
		  pixelSizeUm = 0.0;

	int get_pixel_distance();
	void set_pixel_distance(int pixelDistance);
	size_t get_ram_limit();
	void expand_featuregroups();

	void expand_IMQ_featuregroups();

	static bool gpu_is_available();

	static bool ibsi_compliance;
	static std::string raw_ibsi_compliance; // string for input

	void set_ibsi_compliance(bool skip);

#ifdef USE_GPU
	GpuOptions gpuOptions;
	bool parse_gpu_options_raw_string (const std::string& raw_params_string, std::string& error_message);

	// these are called from Python API's side
	int get_gpu_device_choice();
	void set_gpu_device_id (int id);
	bool using_gpu();	
	void set_using_gpu (bool yes);
	static std::vector<std::map<std::string, std::string>> get_gpu_properties();
#endif

	int get_floating_point_precision();

	int get_coarse_gray_depth();
	void set_coarse_gray_depth(unsigned int new_depth);

	// implementation of SKIPROI
	bool roi_is_blacklisted (const std::string& fname, int roi_label);
	bool parse_roi_blacklist_raw_string (const std::string& raw_blacklist_string, std::string& error_message);
	void clear_roi_blacklist ();
	void get_roi_blacklist_summary(std::string& response);

	bool set_ram_limit(size_t bytes);

	// implementation of Gabor feature options
	bool parse_gabor_options_raw_inputs (std::string& error_message);
	GaborOptions gaborOptions;

	// implementation of GLCM feature options
	bool parse_glcm_options_raw_inputs (std::string& error_message);
	GLCMoptions glcmOptions;

	// implementation of nested ROI options
	bool parse_nested_options_raw_inputs (std::string& error_message);
	NestedRoiOptions nestedOptions;
  
	// implementation of floating point image options
	bool parse_fpimage_options_raw_inputs (std::string& error_message);
	FpImageOptions fpimageOptions;

	std::tuple<bool, std::optional<std::string>> parse_aniso_options_raw_inputs ();
	AnisotropyOptions anisoOptions;

	// implementation of Apache options
	bool arrow_is_enabled();

	// feature result options (yes/no to annotation columns, yes/no to aggregate by slide, NAN substitute, etc)
	ResultOptions resultOptions;
	std::tuple<bool, std::optional<std::string>> parse_result_options_4cli ();

private:

	std::vector<std::tuple<std::string, std::string>> recognizedArgs;	// Accepted command line arguments

	bool find_string_argument (std::vector<std::string>::iterator &i, const char *arg, std::string &arg_value);
	bool find_int_argument(std::vector<std::string>::iterator &i, const char *arg, int &arg_value);
	bool spellcheck_raw_featurelist (const std::string & comma_separated_fnames, std::vector<std::string> & fnames);

	std::string rawTempDirPath = "";

	int floating_point_precision = 10;	

	int coarse_grayscale_depth = 64;
	std::string raw_coarse_grayscale_depth = "";

	// data members implementing RAMLIMIT
	std::string rawRamLimit = "";
	size_t ramLimit = 0;

	// data members implementing TEMPDIR
	std::string rawTempDir = "";

	// implementation of SKIPROI
	std::string rawBlacklistedRois = "";
	RoiBlacklist roiBlacklist;

	// Dataset's dimensionality. Valid values: 2 and 3
	int dim_ = 2;
	std::string raw_dim = "";
	bool expand_2D_featuregroup (const std::string& name);
	bool expand_3D_featuregroup (const std::string& name);
	bool expand_IMQ_featuregroup (const std::string & s);

	bool is_imq_ = false;

	// data members implementing exclusive-inclusive timing switch
	#ifdef CHECKTIMING
		std::string rawExclusiveTiming = "";
	#endif
};

namespace Nyxus
{
	extern Environment theEnvironment;
}

#define VERBOSLVL1(stmt) if(Nyxus::theEnvironment.get_verbosity_level()>=1){stmt;}
#define VERBOSLVL2(stmt) if(Nyxus::theEnvironment.get_verbosity_level()>=2){stmt;}
#define VERBOSLVL3(stmt) if(Nyxus::theEnvironment.get_verbosity_level()>=3){stmt;}
#define VERBOSLVL4(stmt) if(Nyxus::theEnvironment.get_verbosity_level()>=4){stmt;}	
#define VERBOSLVL5(stmt) if(Nyxus::theEnvironment.get_verbosity_level()>=5){stmt;}	
