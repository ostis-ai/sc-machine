#!/usr/bin/env bash

set -eo pipefail

sudo apt-get install lcov

pushd build

lcov -c -i -d . -o base_coverage.info

popd
