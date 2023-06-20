#!/usr/bin/env bash

APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)

export APP_ROOT_PATH="${APP_ROOT_PATH}"
export SC_MACHINE_PATH="${APP_ROOT_PATH}"
export SCRIPTS_PATH="${APP_ROOT_PATH}"/scripts
export BINARY_PATH="${APP_ROOT_PATH}/bin"
export CONFIG_PATH="${APP_ROOT_PATH}/sc-machine.ini"
export REPO_PATH="${APP_ROOT_PATH}/repo.path"
