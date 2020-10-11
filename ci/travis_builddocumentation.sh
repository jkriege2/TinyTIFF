#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

pwd
cd ${TRAVIS_BUILD_DIR}
cd doc
doxygen --version
doxygen Doxyfile
echo "" > html/.nojekyll