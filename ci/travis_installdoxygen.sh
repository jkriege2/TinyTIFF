#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

pwd
doxygen --version
############################################################################
# All the dependencies are installed in ${TRAVIS_BUILD_DIR}/deps/
############################################################################
DEPS_DIR="${TRAVIS_BUILD_DIR}/deps"
mkdir -p ${DEPS_DIR} && cd ${DEPS_DIR}
############################################################################
# Install a recent Doxygen
############################################################################
DOXYGEN_URL="http://doxygen.nl/files/doxygen-1.8.20.linux.bin.tar.gz"
mkdir doxygen
travis_retry wget -O - ${DOXYGEN_URL} | tar --strip-components=1 -xz -C doxygen
export PATH=${DEPS_DIR}/doxygen/bin:${PATH}
doxygen --version