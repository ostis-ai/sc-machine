#!/usr/bin/env bash

set -eo pipefail

sudo apt-get install python3-setuptools
pip3 install --user -r requirements.txt

mkdir build
pushd build

export CC=${CCOMPILER} CXX=${CXXCOMPILER}
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DENABLE_CCACHE=ON -DSC_AUTO_TEST=ON -DSC_KPM_SCP=OFF
echo "travis_fold:start:MAKE"
make -j${JOBS}
echo "travis_fold:end:MAKE"

popd