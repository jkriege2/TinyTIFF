#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX -DTinyTIFF_BUILD_SHARED_LIBS=ON -DTinyTIFF_BUILD_STATIC_LIBS=ON .. 
cmake --build . --target install
