#!/bin/bash 

#=== Request echoing of each command executed 
set -x

cd build-4-linux

rm -r /home/ec2-user/work/data/OUTPUT-tissuenet
mkdir -p /home/ec2-user/work/data/OUTPUT-tissuenet

date
start=$(date +%s)

./nyxus.exe --verbosity=3 --features=MIN --intDir=/home/ec2-user/work/data/tissuenet/intensity --segDir=/home/ec2-user/work/data/tissuenet/labels --outDir=/home/ec2-user/work/data/OUTPUT-tissuenet --filePattern=.* --csvFile=separatecsv --loaderThreads=1 --reduceThreads=8 

end=$(date +%s)
echo "Elapsed Time: $(($end-$start)) seconds"
date

cd ..
