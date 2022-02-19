#pragma once

#include <climits>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "featureset.h"
#include "feature_method.h"
#include "roi_cache.h"

namespace Nyxus
{
	bool datasetDirsOK(const std::string& dirIntens, const std::string& dirLab, const std::string& dirOut, bool mustCheckDirOut);
	bool directoryExists(const std::string& dir);
	void readDirectoryFiles(const std::string& dir, std::vector<std::string>& files);
	bool scanViaFastloader(const std::string& fpath, int num_threads);
	bool scanFilePair(const std::string& intens_fpath, const std::string& label_fpath, int num_FL_threads, int filepair_index, int tot_num_filepairs);
	bool scanFilePairParallel(const std::string& intens_fpath, const std::string& label_fpath, int num_fastloader_threads, int num_sensemaker_threads, int filepair_index, int tot_num_filepairs);
	bool TraverseViaFastloader1(const std::string& fpath, int num_threads);
	std::string getPureFname(std::string fpath);
	int processDataset(const std::vector<std::string>& intensFiles, const std::vector<std::string>& labelFiles, int numFastloaderThreads, int numSensemakerThreads, int numReduceThreads, int min_online_roi_size, bool save2csv, const std::string& csvOutputDir);

	// 2 scenarios of saving a result of feature calculation of a label-intensity file pair: saving to a CSV-file and saving to a matrix to be later consumed by a Python endpoint
	bool save_features_2_csv (std::string intFpath, std::string segFpath, std::string outputDir);
	bool save_features_2_buffer (std::vector<double>& resultMatrix);

	void showCmdlineHelp();
	int read_dataset (
		// input
		const std::string& dirIntens, 
		const std::string& dirLabels, 
		const std::string& dirOut, 
		const std::string& intLabMappingDir,
		const std::string& intLabMappingFile,		
		bool mustCheckDirOut,
		// output
		std::vector<std::string>& intensFiles, std::vector<std::string>& labelFiles);

	void init_feature_buffers();
	
	void update_label(int x, int y, int label, PixIntens intensity);
	void update_label_parallel(int x, int y, int label, PixIntens intensity);

	void print_label_stats();
	void print_by_label(const char* featureName, std::unordered_map<int, StatsInt> L, int numColumns = 8);
	void print_by_label(const char* featureName, std::unordered_map<int, StatsReal> L, int numColumns = 4);
	void clearLabelStats();
	void reduce_by_feature (int nThr, int min_online_roi_size);
	void reduce_by_roi (int nThr, int min_online_roi_size);
	void reduce_trivial_rois (std::vector<int>& PendingRoisLabels);
	void reduce_neighbors();

	void init_label_record(LR& lr, const std::string& segFile, const std::string& intFile, int x, int y, int label, PixIntens intensity);
	void init_label_record_2(LR& lr, const std::string& segFile, const std::string& intFile, int x, int y, int label, PixIntens intensity, unsigned int tile_index);
	void update_label_record(LR& lr, int x, int y, int label, PixIntens intensity);
	void update_label_record_2(LR& lr, int x, int y, int label, PixIntens intensity, unsigned int tile_index);
	void reduce_neighbors(int labels_collision_radius);

	// Timing
	extern double totalImgScanTime, totalFeatureReduceTime;

	// Label data
	extern std::string theSegFname, theIntFname;	// Cached file names while iterating a dataset
	extern std::unordered_set<int> uniqueLabels;
	extern std::unordered_map <int, LR> roiData;
	extern std::vector<double> calcResultBuf;	// [# of labels X # of features]
	extern std::unordered_map <int, std::shared_ptr<std::mutex>> labelMutexes;

	// Feature manager
	extern FeatureMgr theFeatureMgr;

	// System resources
	unsigned long long getAvailPhysMemory();

} // namespace Nyxus

