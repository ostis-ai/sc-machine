#!/usr/bin/env bash
set -eo pipefail

CURRENT_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"
OSTIS_SCRIPTS_DIR="${CURRENT_DIR}/ostis-scripts"
source "${OSTIS_SCRIPTS_DIR}/message-scripts/messages.sh"

warning "This script is deprecated in sc-machine 0.10.0 and will be removed in sc-machine 0.11.0. Use script \`run_sc_machine.sh\` instead."

if [ -z "${BINARY_PATH}" ];
then
  source "${CURRENT_DIR}/set_vars.sh"
fi

"${BINARY_PATH}/sc-server" -c "${CONFIG_PATH}" "$@"
