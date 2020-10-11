#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

cd $CONDA_PREFIX/bin
echo $LD_LIBRARY_PATH
LD_LIBRARY_PATH=$CONDA_PREFIX/lib\:$LD_LIBRARY_PATH ; export LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH
echo $DYLD_LIBRARY_PATH 
DYLD_LIBRARY_PATH =$CONDA_PREFIX/lib\:$DYLD_LIBRARY_PATH  ; export DYLD_LIBRARY_PATH 
echo $DYLD_LIBRARY_PATH 
#./tinytiffwriter_test --simple
./tinytiffreader_test --simple
