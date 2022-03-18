#!/usr/bin/env bash

set -eo pipefail

pushd build

lcov -c -d . -o coverage.info
lcov -a base_coverage.info -a coverage.info -o coverage.info
lcov -r coverage.info "/usr/*" "*/thirdparty/*" -o coverage.info

popd
