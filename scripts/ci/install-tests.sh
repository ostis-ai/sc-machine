#!/usr/bin/env bash

set -eo pipefail

pip3 install --user -r requirements.txt

mkdir build
pushd build

cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DSC_AUTO_TEST=ON -DSC_BUILD_SCTP=OFF -DBUILD_TESTING=ON -DSC_KPM_SCP=OFF
echo "travis_fold:start:MAKE"
make -j4
echo "travis_fold:end:MAKE"

popd