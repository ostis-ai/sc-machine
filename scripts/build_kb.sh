#!/usr/bin/env bash
set -eo pipefail

if [ -z "${BINARY_PATH}" ];
then
  source "$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"/set_vars.sh
fi

"${BINARY_PATH}/sc-builder" -f --clear -c "${CONFIG_PATH}" "$@"
