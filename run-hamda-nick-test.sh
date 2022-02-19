#!/bin/bash 

DATADIR="/home/ec2-user/work/data/hamda-nick-test-nyxus-non-tiled/"
ls -s -a $DATADIR

#=== Request echoing of each command executed 
set -x

cd build-4-linux

./nyxus.exe --verbosity=3 --features=*BASIC_MORPHOLOGY* --intDir=$DATADIR/intensity --segDir=$DATADIR/labels --outDir=/home/ec2-user/work/data/OUTPUT-hamda --filePattern=.* --csvFile=separatecsv --loaderThreads=1 --reduceThreads=4 

cd ..
 

