#!/bin/bash 

### docker run -it --mount type=bind,source=/home/ec2-user/work/data,target=/home/ec2-user/work/data --entrypoint sh labshare/nyxus:0.1.24

docker run -it --mount type=bind,source=/home/ec2-user/work/data,target=/data 214f4c18f595 --intDir=/data/hamda-deep2498/int --segDir=/data/hamda-deep2498/seg --outDir=/data/OUTPUT-hamda --filePattern=.* --csvFile=separatecsv --features=*all* --reduceThreads=4