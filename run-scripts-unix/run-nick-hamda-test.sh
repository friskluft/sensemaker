

/home/ec2-user/work/data/hamda-nick-test-nyxus-non-tiled/intensity


#!/bin/bash 

DATADIR="/home/ec2-user/work/data/hamda-nick-test-nyxus-non-tiled/"
ls $DATADIR

#=== Request echoing of each command executed 
set -x

cd build-4-linux

./nyxus.exe --verbosity=3 --features=*all_intensity* --intDir=$DATADIR/intensity --segDir=$DATADIR/labels --outDir=/home/ec2-user/work/data/OUTPUT-hamda --filePattern=p0_y1_r4_c0_uint16.ome\.tif --csvFile=separatecsv --loaderThreads=1 --reduceThreads=4 

cd ..

