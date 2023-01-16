#!/bin/bash 

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r /home/ec2-user/work/data/OUTPUT_hamda
mkdir -p /home/ec2-user/work/data/OUTPUT_hamda

#--- Run ValGrind--- valgrind --tool=callgrind --main-stacksize=1000000 ./nyxus.exe --features=*all* --intDir=/home/ec2-user/work/data/mini/int --segDir=/home/ec2-user/work/data/mini/seg --outDir=/home/ec2-user/work/data/OUTPUT-mini --filePattern=.* --csvFile=separatecsv --loaderThreads=1 --reduceThreads=1 

./nyxus.exe --features=*basic_morphology* --verbosity=2 --intDir=/home/ec2-user/work/data/hamda_slow/int --segDir=/home/ec2-user/work/data/hamda_slow/seg --outDir=/home/ec2-user/work/data/OUTPUT_hamda --filePattern=.* --csvFile=separatecsv --reduceThreads=4 


cd ..
