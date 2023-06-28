#!/usr/bin/env bash

ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)

export APP_ROOT_PATH="${APP_ROOT_PATH:-${ROOT_PATH}}"
export SC_MACHINE_PATH="${SC_MACHINE_PATH:-${APP_ROOT_PATH}}"
export ROOT_CMAKE_PATH="${ROOT_CMAKE_PATH:-${SC_MACHINE_PATH}}"
export PROBLEM_SOLVER_PATH="${PROBLEM_SOLVER_PATH:-${SC_MACHINE_PATH}}"
export BINARY_PATH="${BINARY_PATH:-${ROOT_CMAKE_PATH}/bin}"
export BUILD_PATH="${BUILD_PATH:-${ROOT_CMAKE_PATH}/build}"
export CONFIG_PATH="${CONFIG_PATH:-${APP_ROOT_PATH}/sc-machine.ini}"
export REPO_PATH="${REPO_PATH:-${APP_ROOT_PATH}/repo.path}"
