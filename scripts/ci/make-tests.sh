#!/usr/bin/env bash

set -eo pipefail

pip3 install --user -r requirements.txt

cmake -B build -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DSC_COVERAGE=${COVERAGE} -DSC_USE_SANITIZER=${SANITIZER_TYPE} -DSC_AUTO_TEST=ON -DSC_BUILD_TESTS=ON
echo ::group::Make
cmake --build build -j$(nproc)
echo ::endgroup::

pushd build
../bin/sc-builder -i ../tools/builder/tests/kb -o ../bin/sc-builder-test-repo -c -f
popd
