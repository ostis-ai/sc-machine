#!/usr/bin/env bash
set -eo pipefail

CURRENT_DIR=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)
source "${CURRENT_DIR}/formats.sh"

if [[ -z "${ROOT_CMAKE_PATH}" || -z "${CXX_SOURCES_PATH}" || -z "${BUILD_PATH}" || -z "${BINARY_PATH}" ]];
then
  source "${CURRENT_DIR}/set_vars.sh"
fi

stage "Build sc-machine"

BUILD_SCRIPTS="${CURRENT_DIR}/build-scripts"
"${BUILD_SCRIPTS}/build_cxx_project.sh" -p "${ROOT_CMAKE_PATH}" -s "${CXX_SOURCES_PATH}" -b "${BUILD_PATH}" \
  --cmake-arg "-DSC_BIN_PATH=${BINARY_PATH}" "$@"

stage "SC-machine built successfully"
