#!/usr/bin/env bash
set -eo pipefail

CURRENT_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"
OSTIS_SCRIPTS_DIR="${CURRENT_DIR}/ostis-scripts"
source "${OSTIS_SCRIPTS_DIR}/message-scripts/messages.sh"

if [[ -z "${ROOT_CMAKE_PATH}" || -z "${CXX_SOURCES_PATH}" || -z "${BUILD_PATH}" || -z "${BINARY_PATH}" ]];
then
  source "${CURRENT_DIR}/set_vars.sh"
fi

ARGS=("$@")
while [ "$1" != "" ];
do
  case $1 in
    "-f"|"--force" )
      build_force=1
      ;;
  esac
  shift 1
done

info "Build sc-machine"

if (( build_force == 1 ));
then
  rm -rf "${BINARY_PATH}"
fi

BUILD_SCRIPTS="${OSTIS_SCRIPTS_DIR}/build-scripts"
"${BUILD_SCRIPTS}/build_cxx_project.sh" -p "${ROOT_CMAKE_PATH}" -s "${CXX_SOURCES_PATH}" -b "${BUILD_PATH}" \
  --cmake-arg "-DSC_BIN_PATH=${BINARY_PATH}" "${ARGS[@]}"

info "sc-machine is built successfully"
