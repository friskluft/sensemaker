#!/bin/bash 

rm -rf !(cmk.sh)
CC=/usr/local/gcc94/bin/gcc
export CC
GXX=/usr/local/gcc94/bin/gcc
export GXX
cmake .. -D BUILD_CLI=yes
make VERBOSE=1
