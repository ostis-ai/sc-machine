#!/usr/bin/env bash
set -eo pipefail

if [[ -z "${BINARY_PATH}" || -z "${BUILD_PATH}" || -z "${SC_MACHINE_PATH}" ]];
then
  source "$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)/set_vars.sh"
fi

"${BINARY_PATH}/sc-builder" -i "${SC_MACHINE_PATH}/sc-tools/sc-builder/tests/repo.path" -o "${BINARY_PATH}/sc-builder-test-repo" --clear -f
"${BINARY_PATH}/sc-builder" -i "${SC_MACHINE_PATH}/sc-tools/sc-server/tests/kb" -o "${BINARY_PATH}/sc-server-test-repo" --clear -f

cd "${BUILD_PATH}" && ctest -C Debug -V
