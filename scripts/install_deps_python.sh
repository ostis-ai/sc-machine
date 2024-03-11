#!/usr/bin/env bash
set -eo pipefail

if [ -z "${SC_MACHINE_PATH}" ];
then
  source "$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)/set_vars.sh"
fi

pip3 install wheel setuptools
pip3 install -r "${SC_MACHINE_PATH}/requirements.txt"

