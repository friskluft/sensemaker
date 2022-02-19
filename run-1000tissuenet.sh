#!/bin/bash 

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r /home/ec2-user/work/data/OUTPUT-tissuenet1000
mkdir -p /home/ec2-user/work/data/OUTPUT-tissuenet1000

./nyxus.exe --features=*all* --intDir=/home/ec2-user/work/data/tissuenet1000/intensity --segDir=/home/ec2-user/work/data/tissuenet1000/labels --outDir=/home/ec2-user/work/data/OUTPUT-tissuenet --filePattern=.* --csvFile=separatecsv --loaderThreads=1 --reduceThreads=8 

cd ..
