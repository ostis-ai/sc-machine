#!/usr/bin/env bash

ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)

export SC_MACHINE_PATH="${SC_MACHINE_PATH:-${ROOT_PATH}}"

export ROOT_CMAKE_PATH="${ROOT_CMAKE_PATH:-${SC_MACHINE_PATH}}"
export PROBLEM_SOLVER_PATH="${PROBLEM_SOLVER_PATH:-${SC_MACHINE_PATH}}" # backward compatibility
export CXX_SOURCES_PATH="${PROBLEM_SOLVER_PATH:-${SC_MACHINE_PATH}}"
export BUILD_PATH="${BUILD_PATH:-${ROOT_CMAKE_PATH}/build}"
export BINARY_PATH="${BINARY_PATH:-${BUILD_PATH}/bin}"
export EXTENSIONS_PATH="${BINARY_PATH:-${BUILD_PATH}/lib}"

export CONFIG_PATH="${CONFIG_PATH:-${SC_MACHINE_PATH}/sc-machine.ini}"
export REPO_PATH="${REPO_PATH:-${SC_MACHINE_PATH}/repo.path}"
