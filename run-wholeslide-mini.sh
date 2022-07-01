#!/bin/bash 

EXPERIMENT_NAME="Wholeslide_mini"
DATADIR="/home/ec2-user/work/data/mini"
OUTDIR="/home/ec2-user/work/data/OUTPUT-mini"
ls -s -a $DATADIR

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r $OUTDIR
mkdir -p $OUTDIR

date
start=$(date +%s)

./nyxus.exe --useGpu=true --verbosity=3 --features=*ALL* --intDir=$DATADIR/int --segDir=$DATADIR/int --outDir=$OUTDIR --filePattern=.* --csvFile=singlecsv --loaderThreads=1 --reduceThreads=8 

end=$(date +%s)
echo "Experiment: $EXPERIMENT_NAME Elapsed Time: $(($end-$start)) seconds"
echo "Experiment: $EXPERIMENT_NAME Elapsed Time: $(($end-$start)) seconds" > timing_$EXPERIMENT_NAME.txt
date

cd ..

#
#
#### GDB #### run --verbosity=0 --features=*all* --intDir=/home/ec2-user/work/data/hamda-stall/int --segDir=/home/ec2-user/work/data/hamda-stall/seg --outDir=/home/ec2-user/work/data/OUTPUT-hamda --filePattern=.* --csvFile=singlecsv --loaderThreads=1 --reduceThreads=8
#
#


