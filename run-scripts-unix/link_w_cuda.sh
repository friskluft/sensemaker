#!/bin/bash 
set -x

/usr/local/gcc94/bin/g++  \
-pthread CMakeFiles/diana.dir/src/nyx/common_stats.cpp.o CMakeFiles/diana.dir/src/nyx/dirs_and_files.cpp.o CMakeFiles/diana.dir/src/nyx/environment.cpp.o \
CMakeFiles/diana.dir/src/nyx/environment_basic.cpp.o CMakeFiles/diana.dir/src/nyx/feature_method.cpp.o CMakeFiles/diana.dir/src/nyx/feature_mgr.cpp.o \
CMakeFiles/diana.dir/src/nyx/feature_mgr_init.cpp.o CMakeFiles/diana.dir/src/nyx/features/basic_morphology.cpp.o CMakeFiles/diana.dir/src/nyx/features/caliper_feret.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/caliper_martin.cpp.o CMakeFiles/diana.dir/src/nyx/features/caliper_nassenstein.cpp.o CMakeFiles/diana.dir/src/nyx/features/chords.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/chords_nontriv.cpp.o CMakeFiles/diana.dir/src/nyx/features/circle.cpp.o CMakeFiles/diana.dir/src/nyx/features/contour.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/convex_hull_nontriv.cpp.o CMakeFiles/diana.dir/src/nyx/features/ellipse_fitting.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/erosion_pixels.cpp.o CMakeFiles/diana.dir/src/nyx/features/euler_number.cpp.o CMakeFiles/diana.dir/src/nyx/features/extrema.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/fractal_dim.cpp.o CMakeFiles/diana.dir/src/nyx/features/gabor.cpp.o CMakeFiles/diana.dir/src/nyx/features/gabor_nontriv.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/geo_len_thickness.cpp.o CMakeFiles/diana.dir/src/nyx/features/glcm.cpp.o CMakeFiles/diana.dir/src/nyx/features/glcm_nontriv.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/gldm.cpp.o CMakeFiles/diana.dir/src/nyx/features/glrlm.cpp.o CMakeFiles/diana.dir/src/nyx/features/glszm.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/hexagonality_polygonality.cpp.o CMakeFiles/diana.dir/src/nyx/features/histogram.cpp.o CMakeFiles/diana.dir/src/nyx/features/image_matrix.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/image_matrix_nontriv.cpp.o CMakeFiles/diana.dir/src/nyx/features/image_moments.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/image_moments_nontriv.cpp.o CMakeFiles/diana.dir/src/nyx/features/intensity.cpp.o CMakeFiles/diana.dir/src/nyx/features/neighbors.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/ngtdm.cpp.o CMakeFiles/diana.dir/src/nyx/features/pixel_intensity.cpp.o CMakeFiles/diana.dir/src/nyx/features/radial_distribution.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/roi_label.cpp.o CMakeFiles/diana.dir/src/nyx/features/roi_radius.cpp.o CMakeFiles/diana.dir/src/nyx/features/rotation.cpp.o \
CMakeFiles/diana.dir/src/nyx/features/specfunc.cpp.o CMakeFiles/diana.dir/src/nyx/features/zernike.cpp.o CMakeFiles/diana.dir/src/nyx/features/zernike_nontriv.cpp.o \
CMakeFiles/diana.dir/src/nyx/features_calc_workflow.cpp.o CMakeFiles/diana.dir/src/nyx/featureset.cpp.o CMakeFiles/diana.dir/src/nyx/globals.cpp.o \
CMakeFiles/diana.dir/src/nyx/helpers/timing.cpp.o CMakeFiles/diana.dir/src/nyx/image_loader.cpp.o CMakeFiles/diana.dir/src/nyx/image_loader1x.cpp.o \
CMakeFiles/diana.dir/src/nyx/main_nyxus.cpp.o CMakeFiles/diana.dir/src/nyx/nested_roi.cpp.o CMakeFiles/diana.dir/src/nyx/output_2_buffer.cpp.o \
CMakeFiles/diana.dir/src/nyx/output_2_csv.cpp.o CMakeFiles/diana.dir/src/nyx/parallel.cpp.o CMakeFiles/diana.dir/src/nyx/phase1.cpp.o \
CMakeFiles/diana.dir/src/nyx/phase2.cpp.o CMakeFiles/diana.dir/src/nyx/phase3.cpp.o CMakeFiles/diana.dir/src/nyx/reduce_by_feature.cpp.o \
CMakeFiles/diana.dir/src/nyx/reduce_trivial_rois.cpp.o CMakeFiles/diana.dir/src/nyx/roi_cache.cpp.o CMakeFiles/diana.dir/src/nyx/roi_cache_basic.cpp.o \
CMakeFiles/diana.dir/src/nyx/scan_fastloader_way.cpp.o \
-L/usr/local/cuda/targets/x86_64-linux/lib/stubs  \
-L/usr/local/cuda/targets/x86_64-linux/lib  \
-L/usr/lib/gcc/x86_64-redhat-linux/7_xxxxx  \
libparticles.a \
-static-libstdc++ \
-lm -ltiff -lfftw3 \
-lpthread \
-ldl -lrt \
-o diana 

#-lstdc++fs \

#-L/usr/local/cuda/targets/x86_64-linux/lib/stubs  \
#-L/usr/local/cuda/targets/x86_64-linux/lib  \
#-L/usr/lib/gcc/x86_64-redhat-linux/7  \
#-lpthread /usr/local/lib/libtiff.so \
#-lstdc++ -static-libgcc -static-libstdc++ \
#-lcudadevrt -lcudart_static -lrt -lpthread -ldl	\
