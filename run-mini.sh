#!/bin/bash 

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r /home/ec2-user/work/data/OUTPUT-mini
mkdir -p /home/ec2-user/work/data/OUTPUT-mini

#--- Run ValGrind--- valgrind --tool=callgrind --main-stacksize=1000000 ./nyxus.exe --features=*all* --intDir=/home/ec2-user/work/data/mini/int --segDir=/home/ec2-user/work/data/mini/seg --outDir=/home/ec2-user/work/data/OUTPUT-mini --filePattern=.* --csvFile=separatecsv --loaderThreads=1 --reduceThreads=1 
./nyxus.exe --features=*all* --intDir=/home/ec2-user/work/data/mini/int --segDir=/home/ec2-user/work/data/mini/seg --outDir=/home/ec2-user/work/data/OUTPUT-mini --filePattern=.* --csvFile=separatecsv --loaderThreads=1 --reduceThreads=1 


cd ..
