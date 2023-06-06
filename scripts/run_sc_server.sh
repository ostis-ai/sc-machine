#!/bin/bash
set -e

if [[ -z ${BINARY_PATH+1} ]];
then
  source "$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"/set_vars.sh
fi

"${BINARY_PATH}"/sc-server -c "${CONFIG_PATH}" "$@"
