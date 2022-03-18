#!/usr/bin/env bash

set -eo pipefail

pushd build
ctest -C Debug -V

popd
