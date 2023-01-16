#!/bin/bash 

#=== Request echoing of each command executed 
set -x

cd build-4-linux

./nyxus.exe --features=*BASIC_MORPHOLOGY* --intDir=/home/ec2-user/work/data/hamda-deep2498/int --segDir=/home/ec2-user/work/data/hamda-deep2498/seg --outDir=/home/ec2-user/work/data/OUTPUT-hamda --filePattern=.* --csvFile=separatecsv --loaderThreads=2 --reduceThreads=8 

cd ..
