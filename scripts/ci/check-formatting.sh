#!/usr/bin/env bash

set -eo pipefail

mkdir build -p
pushd build
cmake .. -DSC_CLANG_FORMAT_CODE=ON
make clangformat_check
popd
