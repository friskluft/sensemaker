#include "environment.h"
#include "globals.h"
#include "helpers/timing.h"

std::map <std::string, double> Stopwatch::totals;

namespace Nyxus
{
	FeatureSet theFeatureSet;
	FeatureMgr theFeatureMgr;
	std::string theSegFname, theIntFname;
	std::unordered_set <int> uniqueLabels;
	std::unordered_map <int, LR> roiData;
	std::unordered_map <int, std::shared_ptr<std::mutex>> labelMutexes;
	std::vector<double> calcResultBuf;
	Environment theEnvironment;

	// Timing
	double totalImgScanTime = 0.0, totalFeatureReduceTime = 0.0;	// Time counters external to class 'Stopwatch'
}
