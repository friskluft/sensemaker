#!/bin/bash 

#=== Request echoing of each command executed 
set -x

cd build-4-linux
#	rm -r ./output-j/*
#	mkdir -p output-j

./nyxus.exe --features=*all* --intDir=/home/ec2-user/work/data/data-jayapriya/intensity --segDir=/home/ec2-user/work/data/data-jayapriya/labels --outDir=/home/ec2-user/work/data/OUTPUT-jayapriya --filePattern=.* --csvFile=singlecsv --loaderThreads=2 --reduceThreads=8

cd ..

