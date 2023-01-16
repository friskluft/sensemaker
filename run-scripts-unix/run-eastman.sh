#!/bin/bash 

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r /home/ec2-user/work/data/OUTPUT-eastman
mkdir -p /home/ec2-user/work/data/OUTPUT-eastman

./nyxus.exe --features=*all_intensity* --verbosity=2 --intDir=/home/ec2-user/work/data/eastman/int --segDir=/home/ec2-user/work/data/eastman/seg --outDir=/home/ec2-user/work/data/OUTPUT-eastman --filePattern=.* --csvFile=separatecsv --loaderThreads=1 --reduceThreads=8

cd ..
