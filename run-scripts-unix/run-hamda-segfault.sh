#!/bin/bash 

EXPERIMENT_NAME="Nyx_vs_Cellprofiler"
DATADIR="/home/ec2-user/work/data/hamda-segfault"
OUTDIR="/home/ec2-user/work/data/OUTPUT-hamda"
ls -s -a $DATADIR

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r $OUTDIR
mkdir -p $OUTDIR

date
start=$(date +%s)

./nyxus.exe --verbosity=3 --features=*ALL* --intDir=$DATADIR/int --segDir=$DATADIR/seg --outDir=$OUTDIR --filePattern=.* --csvFile=singlecsv --loaderThreads=1 --reduceThreads=8 

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


