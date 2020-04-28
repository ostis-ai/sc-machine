#!/usr/bin/env bash

set -eo pipefail

pushd bin

echo "travis_fold:start:TEST_SCS"
./test_scs
echo "travis_fold:end:TEST_SCS"

echo "travis_fold:start:TEST_MEMORY"
./sc-memory-tests
echo "travis_fold:end:TEST_MEMORY"

popd

echo "travis_fold:start:TEST_BUILDER"
pushd scripts/tests
./builder_test_kb.sh
popd
pushd bin
./sc-builder-test
popd
echo "travis_fold:end:TEST_BUILDER"