#!/usr/bin/env bash
set -eo pipefail

SC_MACHINE_PATH="$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)"

python3 -m venv "${SC_MACHINE_PATH}/.venv"
source "${SC_MACHINE_PATH}/.venv/bin/activate"
pip3 install wheel setuptools
pip3 install -r "${SC_MACHINE_PATH}/requirements.txt"
