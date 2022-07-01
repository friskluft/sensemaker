#!/bin/bash 

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r /home/ec2-user/work/data/OUTPUT-tissuenet
mkdir -p /home/ec2-user/work/data/OUTPUT-tissuenet

date
start=$(date +%s)

### Implementing area_none,centroid_x,centroid_y,bbox_xmin,bbox_ymin,bbox_width,bbox_height,major_axis_length_none,minor_axis_length_none,eccentricity,orientation,convex_area_none,euler_number,equivalent_diameter_none,solidity,perimeter_none,maxferet_none,minferet_none,neighbors,polygonality_score,hexagonality_score,hexagonality_sd,kurtosis,maximum_intensity,mean_intensity,median,minimum_intensity,mode,standard_deviation,skewness,touching_border

./nyxus.exe --verbosity=0 --intDir=/home/ec2-user/work/data/tissuenet/intensity --segDir=/home/ec2-user/work/data/tissuenet/labels --outDir=/home/ec2-user/work/data/OUTPUT-tissuenet-nyxasjaya --filePattern=.* --csvFile=separatecsv --loaderThreads=1 --reduceThreads=8 --features=*ALL_INTENSITY*,*ALL_MORPHOLOGY*,NUM_NEIGHBORS,PERCENT_TOUCHING



end=$(date +%s)
echo "Elapsed Time: $(($end-$start)) seconds"
date

cd ..

