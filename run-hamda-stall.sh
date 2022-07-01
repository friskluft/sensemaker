#!/bin/bash 

DATADIR="/home/ec2-user/work/data/hamda-stall"
OUTDIR="/home/ec2-user/work/data/OUTPUT-hamda"
ls -s -a $DATADIR

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r /home/ec2-user/work/data/OUTPUT-hamda
mkdir -p /home/ec2-user/work/data/OUTPUT-hamda

date
start=$(date +%s)

./nyxus.exe --verbosity=3 --features=*all* --intDir=$DATADIR/int --segDir=$DATADIR/int --outDir=$OUTDIR --filePattern=.* --csvFile=singlecsv --loaderThreads=1 --reduceThreads=8 

end=$(date +%s)
echo "Elapsed Time: $(($end-$start)) seconds"
date

cd ..

#### GDB #### run --verbosity=3 --features=*all* --intDir=/home/ec2-user/work/data/hamda-stall/int --segDir=/home/ec2-user/work/data/hamda-stall/seg --outDir=/home/ec2-user/work/data/OUTPUT-hamda --filePattern=.* --csvFile=singlecsv --loaderThreads=1 --reduceThreads=8



