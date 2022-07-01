#!/bin/bash 

EXPERIMENT_NAME="Segmented_Tissuenet1000"
DATADIR="/home/ec2-user/work/data/tissuenet1000"
OUTDIR="/home/ec2-user/work/data/OUTPUT-tissuenet1000"
ls -s -a $DATADIR

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r $OUTDIR
mkdir -p $OUTDIR

date
start=$(date +%s)

./nyxus.exe --useGpu=true --verbosity=3 --features=*ALL* --intDir=$DATADIR/int --segDir=$DATADIR/seg --outDir=$OUTDIR --filePattern=.* --csvFile=singlecsv --loaderThreads=1 --reduceThreads=8 

end=$(date +%s)
echo "Experiment: $EXPERIMENT_NAME Elapsed Time: $(($end-$start)) seconds"
echo "Experiment: $EXPERIMENT_NAME Elapsed Time: $(($end-$start)) seconds" > timing_$EXPERIMENT_NAME.txt
date

cd ..

