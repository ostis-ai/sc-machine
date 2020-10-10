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

echo "travis_fold:start:builder_kb"
../bin/sc-builder -i ../tools/builder/tests/kb -o ../bin/sc-builder-test-repo -c -f
echo "travis_fold:end:builder_kb"

popd