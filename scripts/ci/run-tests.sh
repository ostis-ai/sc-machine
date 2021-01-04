#!/usr/bin/env bash

set -eo pipefail

pushd build

echo "travis_fold:start:TEST"
ctest -C Debug -V
echo "travis_fold:end:TEST"

popd
