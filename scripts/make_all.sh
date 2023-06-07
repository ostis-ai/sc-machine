#!/usr/bin/env bash
set -eo pipefail

YELLOW='\033[01;33m'
NC='\033[0m' # No Color
echo -e "${YELLOW}[WARNING] This script was deprecated in sc-machine 0.8.0.
Please, use scripts/build_sc_machine.sh instead. It will be removed in sc-machine 0.9.0.${NC}"

if [[ -z ${BINARY_PATH+1} ]];
then
  source "$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"/set_vars.sh
fi

cmake -B "${APP_ROOT_PATH}/build" "${APP_ROOT_PATH}" "$@"
cmake --build "${APP_ROOT_PATH}/build" -j$(nproc)
