#include "helpers/fsystem.h"
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <map>
#include <array>
#include <regex>
#include <string>
#include <limits>

#include <future>
#include <thread>

#ifdef WITH_PYTHON_H
	#include <pybind11/pybind11.h>
	#include <pybind11/stl.h>
	#include <pybind11/numpy.h>
	namespace py = pybind11;
#endif

#include "dirs_and_files.h"
#include "environment.h"
#include "features/contour.h"
#include "features/erosion.h"
#include "features/gabor.h"
#include "features/2d_geomoments.h"
#include "globals.h"
#include "helpers/helpers.h"
#include "helpers/system_resource.h"
#include "helpers/timing.h"

#include "raw_image_loader.h"
#include "save_option.h"

#ifdef USE_GPU
	#include "gpucache.h"
#endif


	namespace Nyxus
	{
		bool scan_intlabel_pair_props (SlideProps& p);
		bool processIntSegImagePair (const std::string& intens_fpath, const std::string& label_fpath, int num_FL_threads, int filepair_index, int tot_num_filepairs);
		bool process_whole_slide (const std::string& intens_fpath, ImageLoader & imlo, LR & fakeroi);

		static std::mutex mutexApache1;

		void handler1 (
			const std::vector<std::string> & intensFiles, 
			const std::vector<std::string> & labelFiles, 
			size_t i, 
			size_t nf, 
			const std::string & outputPath, 
			bool write_apache, 
			Nyxus::SaveOption saveOption,
			int & rv)
		{
			#ifdef CHECKTIMING
			if (Stopwatch::exclusive())
				Stopwatch::reset();
			#endif

			// Clear ROI data cached for the previous image
			clear_slide_rois();

			SlideProps& p = LR::dataset_props[i];

			// Scan one label-intensity pair 
			ImageLoader imlo;
			bool ok = imlo.open(p);	//??????????????????? theImLoader.open(p);
			if (ok == false)
			{
				std::cerr << "Terminating\n";
				rv = 1;
			}

			// Do phased processing: prescan, trivial ROI processing, oversized ROI processing
			LR fakeroi;
			ok = process_whole_slide (p.fname_int, imlo, fakeroi); //?????????????? needs to be replaced with something specialized wholeslide ?????????    ok = processIntSegImagePair (ifp, lfp, 1/* ???????????? numFastloaderThreads*/, i, nf);

			VERBOSLVL2(std::cout << "@@@handler1() saving features\n");

			if (ok == false)
			{
				std::cout << "processIntSegImagePair() returned an error code while processing slide " << p.fname_int << "\n";
				rv = 1;
			}

			if (write_apache) 
			{
				VERBOSLVL2(std::cout << "@@@handler1() saveOption == write_apache\n");

				std::lock_guard<std::mutex> lg(mutexApache1);

				auto [status, msg] = theEnvironment.arrow_stream.write_arrow_file (
					Nyxus::get_feature_values_roi (fakeroi, p.fname_int, ""));

				if (!status) 
				{
					std::cout << "Error writing Arrow file: " << msg.value() << std::endl;
					rv = 2;
				}
			}
			else 
				if (saveOption == SaveOption::saveCSV)
				{
					VERBOSLVL2(std::cout << "@@@handler1() saveOption == SaveOption::saveCSV\n");

					ok = save_features_2_csv_roi (fakeroi, p.fname_int, "", outputPath); // true; //??????????? !!!! save_features_2_csv(p.fname_int, p.fname_int, outputPath);

					if (ok == false)
					{
						std::cout << "save_features_2_csv() returned an error code" << std::endl;
						rv = 2;
					}
				}
				else
				{
					VERBOSLVL2(std::cout << "@@@handler1() saveOption == buffer\n");

					ok = save_features_2_buffer_roi (theResultsCache, fakeroi, p.fname_int, "");

					if (ok == false)
					{
						std::cout << "save_features_2_buffer() returned an error code" << std::endl;
						rv = 2;
					}
				}

			VERBOSLVL2(std::cout << "@@@handler1() done saving features\n");

			theImLoader.close();

			// Save nested ROI related info of this image
			if (theEnvironment.nestedOptions.defined())
				save_nested_roi_info(nestedRoiData, uniqueLabels, roiData);

#ifdef WITH_PYTHON_H
			// Allow heyboard interrupt.
			if (PyErr_CheckSignals() != 0)
			{
				sureprint("\nAborting per user input\n");
				throw pybind11::error_already_set();
			}
#endif

			#ifdef CHECKTIMING
			if (Stopwatch::exclusive())
			{
				// Detailed timing - on the screen
				VERBOSLVL1(Stopwatch::print_stats());

				// Details - also to a file
				//xxx		VERBOSLVL1(
				//xxx			fs::path p(theSegFname);
				//xxx			Stopwatch::save_stats(theEnvironment.output_dir + "/" + p.stem().string() + "_nyxustiming.csv")
				//xxx				);
			}
			#endif

			rv = 0; // success

			VERBOSLVL2(std::cout << "@@@leaving handler1()\n");

		}

		typedef void (*functype1) (
			size_t start, size_t end, 
			//std::vector<int>* ptrLabels, std::unordered_map <int, LR>* ptrLabelData
			const std::vector<std::string>& intensFiles,
			const std::vector<std::string>& labelFiles,
			int n_threads,
			int min_online_roi_size,
			const SaveOption saveOption,
			const std::string& outputPath);

		inline void runParallel1 (functype1 f, int nThr, size_t workPerThread, size_t datasetSize, 
			//std::vector<int>* ptrLabels, std::unordered_map <int, LR>* ptrLabelData
			const std::vector<std::string>& intensFiles,
			const std::vector<std::string>& labelFiles,
			int n_threads,
			int min_online_roi_size,
			const SaveOption saveOption,
			const std::string& outputPath
		)
		{
			std::vector<std::future<void>> T;
			for (int t = 0; t < nThr; t++)
			{
				size_t idxS = t * workPerThread,
					idxE = idxS + workPerThread;
				if (t == nThr - 1)
					idxE = datasetSize; // include the tail
				T.push_back(std::async(std::launch::async, f, idxS, idxE, 
					//ptrLabels, ptrLabelData
					intensFiles,
					labelFiles,
					n_threads,
					min_online_roi_size,
					saveOption,
					outputPath
					));
			}
		}

		int processDataset_wholeslide(
			const std::vector<std::string>& intensFiles,
			const std::vector<std::string>& labelFiles,
			int n_threads,
			int min_online_roi_size,
			const SaveOption saveOption,
			const std::string& outputPath)
		{

#ifdef CHECKTIMING
			if (Stopwatch::inclusive())
				Stopwatch::reset();
#endif		

			//********************** prescan ***********************

			// slide properties
			size_t nf = intensFiles.size();

			{ STOPWATCH("prescan/p0/P/#ccbbaa", "\t=");

			VERBOSLVL1(std::cout << "phase 0 (prescanning)\n");

			LR::reset_dataset_props();
			LR::dataset_props.resize(nf);
			for (size_t i = 0; i < nf; i++)
			{
				// slide file names
				SlideProps& p = LR::dataset_props[i];
				p.fname_int = intensFiles[i];
				p.fname_seg = labelFiles[i];

				// slide metrics
				VERBOSLVL1(std::cout << "prescanning " << p.fname_int);
				if (!scan_intlabel_pair_props(p))
				{
					VERBOSLVL1(std::cout << "error prescanning pair " << p.fname_int << " and " << p.fname_seg << std::endl);
					return 1;
				}
				VERBOSLVL1(std::cout << "\tmax ROI " << p.max_roi_w << " x " << p.max_roi_h << "\tmin-max I " << p.min_preroi_inten << "-" << p.max_preroi_inten << "\n");
			}

			// global properties
			LR::dataset_max_combined_roicloud_len = 0;
			LR::dataset_max_n_rois = 0;
			LR::dataset_max_roi_area = 0;
			LR::dataset_max_roi_w = 0;
			LR::dataset_max_roi_h = 0;

			for (SlideProps& p : LR::dataset_props)
			{
				size_t sup_s_n = p.n_rois * p.max_roi_area;
				LR::dataset_max_combined_roicloud_len = (std::max)(LR::dataset_max_combined_roicloud_len, sup_s_n);
				LR::dataset_max_n_rois = (std::max)(LR::dataset_max_n_rois, p.n_rois);
				LR::dataset_max_roi_area = (std::max)(LR::dataset_max_roi_area, p.max_roi_area);
				LR::dataset_max_roi_w = (std::max)(LR::dataset_max_roi_w, p.max_roi_w);
				LR::dataset_max_roi_h = (std::max)(LR::dataset_max_roi_h, p.max_roi_h);
			}

			VERBOSLVL1(std::cout << "\t finished prescanning \n");

			//********************** allocate the GPU cache ***********************

#ifdef USE_GPU
			if (theEnvironment.using_gpu())
			{
				// what parts of GPU cache we need to bother about ?
				bool needContour = ContourFeature::required(theFeatureSet),
					needErosion = ErosionPixelsFeature::required(theFeatureSet),
					needGabor = GaborFeature::required(theFeatureSet),
					needImoments = Imoms2D_feature::required(theFeatureSet),
					needSmoments = Smoms2D_feature::required(theFeatureSet),
					needMoments = needImoments || needSmoments;

				// whole slide's contour is just 4 vertices long
				size_t kontrLen = Nyxus::theEnvironment.singleROI ? 4 : LR::dataset_max_combined_roicloud_len;

				if (theEnvironment.using_gpu())
				{
					// allocate
					VERBOSLVL1(std::cout << "allocating GPU cache \n");

					if (!NyxusGpu::allocate_gpu_cache(
						// out
						NyxusGpu::gpu_roiclouds_2d,
						NyxusGpu::gpu_roicontours_2d,
						&NyxusGpu::dev_realintens,
						&NyxusGpu::dev_prereduce,
						NyxusGpu::gpu_featurestatebuf,
						NyxusGpu::devicereduce_temp_storage_szb,
						&NyxusGpu::dev_devicereduce_temp_storage,
						NyxusGpu::gpu_batch_len,
						&NyxusGpu::dev_imat1,
						&NyxusGpu::dev_imat2,
						NyxusGpu::gabor_linear_image,
						NyxusGpu::gabor_linear_kernel,
						NyxusGpu::gabor_result,
						NyxusGpu::gabor_energy_image,
						// in
						needContour,
						needErosion,
						needGabor,
						needMoments,
						LR::dataset_max_combined_roicloud_len, // desired totCloLen,
						kontrLen, // desired totKontLen,
						LR::dataset_max_n_rois,	// labels.size()
						LR::dataset_max_roi_area,
						LR::dataset_max_roi_w,
						LR::dataset_max_roi_h,
						GaborFeature::f0_theta_pairs.size(),
						GaborFeature::n
					))	// we need max ROI area inside the function to calculate the batch size if 'dataset_max_combined_roicloud_len' doesn't fit in RAM
					{
						std::cerr << "error in " << __FILE__ << ":" << __LINE__ << "\n";
						return 1;
					}

					VERBOSLVL1(std::cout << "\t ---done allocating GPU cache \n");
				}
			} // opted to use GPU
#endif
			} // prescan timing

			//????????????????	// One-time initialization
			//????????????????	init_slide_rois();

			bool write_apache = (saveOption == SaveOption::saveArrowIPC || saveOption == SaveOption::saveParquet);

			// initialize arrow writer if needed
			if (write_apache) 
			{

				theEnvironment.arrow_stream = ArrowOutputStream();
				std::string afn = get_arrow_filename (outputPath, theEnvironment.nyxus_result_fname, saveOption);
				VERBOSLVL2 (std::cout << "@@@ Arrow file name =" << afn << "\n");
				auto [status, msg] = theEnvironment.arrow_stream.create_arrow_file(
					saveOption,
					afn,
					Nyxus::get_header(theFeatureSet.getEnabledFeatures()));

				if (!status) {
					std::cout << "Error creating Arrow file: " << msg.value() << std::endl;
					return 1;
				}
			}

			bool ok = true;

//???????????? v2
			size_t n_jobs = (nf + n_threads - 1) / n_threads;
			for (size_t j=0; j<n_jobs; j++)
			{
				std::cout << "job " << j+1 << "/" << n_jobs << "\n";

				std::vector<std::future<void>> T;
				for (int t=0; t < n_threads; t++)
				{
					size_t idx = j * n_threads + t;

					// done?
					if (idx + 1 > nf)
						break;

					int rval = 0;
					T.push_back (std::async(std::launch::async, 
						handler1, 
						intensFiles,
						labelFiles,
						idx,
						nf,
						outputPath,
						write_apache,
						saveOption,
						std::ref(rval)));
				}

			}

			VERBOSLVL2(std::cout << "@@@ after running jobs 1\n");

#ifdef CHECKTIMING
			if (Stopwatch::inclusive())
			{
				// Detailed timing - on the screen
				VERBOSLVL1(Stopwatch::print_stats());

				// Details - also to a file
				VERBOSLVL1(
					fs::path p(theSegFname);
				Stopwatch::save_stats(theEnvironment.output_dir + "/inclusive_nyxustiming.csv");
				);
			}
#endif

			if (write_apache) 
			{
				// close arrow file after use
				auto [status, msg] = theEnvironment.arrow_stream.close_arrow_file();
				if (!status) {
					std::cout << "Error closing Arrow file: " << msg.value() << std::endl;
					return 2;
				}
			}

#ifdef USE_GPU
			if (theEnvironment.using_gpu())
			{
				if (!NyxusGpu::free_gpu_cache(
					NyxusGpu::gpu_roiclouds_2d,
					NyxusGpu::gpu_roicontours_2d,
					NyxusGpu::dev_realintens,
					NyxusGpu::dev_prereduce,
					NyxusGpu::gpu_featurestatebuf,
					NyxusGpu::dev_devicereduce_temp_storage,
					NyxusGpu::dev_imat1,
					NyxusGpu::dev_imat2,
					NyxusGpu::gabor_linear_image,
					NyxusGpu::gabor_result,
					NyxusGpu::gabor_linear_kernel,
					NyxusGpu::gabor_energy_image))
				{
					std::cerr << "error in free_gpu_cache()\n";
					return 1;
				}
			}
#endif

			VERBOSLVL2(std::cout << "@@@ after running jobs 2\n");

			return 0; // success
		}

	}

