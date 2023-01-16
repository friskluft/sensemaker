#!/bin/bash 

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r /home/ec2-user/work/data/OUTPUT-eastman2022
mkdir -p /home/ec2-user/work/data/OUTPUT-eastman2022

./nyxus.exe --features=*all_intensity* --intDir=/home/ec2-user/work/data/eastman2022/int --segDir=/home/ec2-user/work/data/eastman2022/seg --outDir=/home/ec2-user/work/data/OUTPUT-eastman2022 --filePattern=.* --csvFile=separatecsv --loaderThreads=1 --reduceThreads=8

cd ..
