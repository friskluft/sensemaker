#!/bin/bash 

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r OUTPUT-ratbrain/*
mkdir -p OUTPUT-ratbrain

./nyxus.exe --features=*all* --intDir=/home/ec2-user/work/data/data-ratbrain/int --segDir=/home/ec2-user/work/data/data-ratbrain/seg --outDir=/home/ec2-user/work/data/OUTPUT-ratbrain --filePattern=.* --csvFile=singlecsv --loaderThreads=1 --reduceThreads=1 

cd ..
