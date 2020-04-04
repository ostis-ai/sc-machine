#!/usr/bin/env bash

set -eo pipefail

pip3 install --user -r requirements.txt

mkdir build
pushd build

export CC=${CCOMPILER} CXX=${CXXCOMPILER}
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DSC_AUTO_TEST=ON -DSC_BUILD_TESTS=ON -DSC_KPM_SCP=OFF
echo "travis_fold:start:MAKE"
make -j${JOBS}
echo "travis_fold:end:MAKE"

popd