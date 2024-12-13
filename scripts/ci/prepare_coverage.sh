#!/usr/bin/env bash

set -eo pipefail

sudo apt-get install lcov

lcov -c -i -d . -o base_coverage.info
