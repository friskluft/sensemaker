#!/bin/bash 

DATADIR="/home/ec2-user/work/data/hamda-neighbors2/6234838c6b123e21c8b736f5/tissuenet_tif"
OUTDIR="/home/ec2-user/work/data/OUTPUT-hamda"
ls -s -a $DATADIR
FPATT=".*"

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r $OUTDIR
mkdir -p $OUTDIR

date
start=$(date +%s)

./nyxus.exe --verbosity=3 --features=*all* --intDir=$DATADIR/int --segDir=$DATADIR/seg --outDir=$OUTDIR --filePattern=$FPATT --csvFile=singlecsv --loaderThreads=1 --reduceThreads=8 

end=$(date +%s)
echo "Elapsed Time: $(($end-$start)) seconds"
date

cd ..

#### GDB #### run --verbosity=3 --features=*all* --intDir=/home/ec2-user/work/data/hamda-stall/int --segDir=/home/ec2-user/work/data/hamda-stall/seg --outDir=/home/ec2-user/work/data/OUTPUT-hamda --filePattern=.* --csvFile=singlecsv --loaderThreads=1 --reduceThreads=8



