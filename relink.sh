#!/bin/bash 

#=== Request echoing of each command executed 
set -x

# set OPT = "-c -Ofast" 
# echo "$OPT"

CXX=/usr/local/gcc94/bin/gcc
export CXX

GXX=/usr/local/gcc94/bin/g++
export GXX

INCLU='-I /home/ec2-user/work/prep-fastloader/FastLoader-main -I /home/ec2-user/work/prep-hedgehog/hedgehog-master -I /home/ec2-user/work/sensemaker4-nyx/lib/pybind11/include -I /home/ec2-user/gcc_install/gcc-9.4.0/isl-0.18/interface '
export INCLU

BUILDDIR=./build-4-linux
export BUILDDIR

OPTS='-c -std=c++17 -D CHECKTIMING -g '
export OPTS

#=== Prepare the build output directory
############### mkdir -p $BUILDDIR
############### rm $BUILDDIR/*
cd $BUILDDIR

#=== We're in the 'build' directory so all the source files are in '../'
######### ... $CXX $OPTS $INCLU ../src/nyx/scan_fastloader_way.cpp

$GXX \
-Wl,-s,10485760 \
erosion_pixels.o \
geo_len_thickness.o \
hexagonality_polygonality.o \
particle_metrics.o \
roi_label.o \
chords.o \
convex_hull.o \
euler_number.o \
fractal_dim.o \
glcm.o \
glszm.o \
histogram.o \
roi_radius.o \
zernike.o \
neighbors.o \
circle.o \
ellipse_fitting.o \
extrema.o \
gabor.o \
gldm.o \
image_matrix.o \
pixel_intensity.o \
rotation.o \
haralick_texture.o \
ngtdm.o \
radial_distribution.o \
contour.o \
glrlm.o \
image_moments.o \
specfunc.o \
system_resource.o \
timing.o \
common_stats.o \
environment.o \
features_calc_workflow.o \
globals.o \
output_2_buffer.o \
parallel.o \
reduce_by_roi.o \
roi_cache.o \
dirs_and_files.o \
feature_method.o \
featureset.o \
main.o \
output_2_csv.o \
reduce_by_feature.o \
reduce_trivial_rois.o \
scan_fastloader_way.o \
-lm -ltiff -lfftw3 \
-lpthread \
-lpthread -static-libstdc++ \
-o nyxus.exe

cd .. # Leave BUILDDIR

ls -la $BUILDDIR | grep nyxus.exe 

