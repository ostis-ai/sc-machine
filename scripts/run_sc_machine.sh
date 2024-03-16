#!/usr/bin/env bash
set -eo pipefail

CURRENT_DIR=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)
if [ -z "${BINARY_PATH}" ];
then
  source "${CURRENT_DIR}/set_vars.sh"
fi

"${BINARY_PATH}/sc-machine" -c "${CONFIG_PATH}" "$@"
