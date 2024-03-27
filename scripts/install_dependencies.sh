#!/usr/bin/env bash
set -eo pipefail

CURRENT_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"
OSTIS_SCRIPTS_DIR="${CURRENT_DIR}/ostis-scripts"
source "${OSTIS_SCRIPTS_DIR}/message-scripts/messages.sh"

unameOut="$(uname -s)"
case "${unameOut}" in
  Linux*)
    machine=Linux
    ;;
  Darwin*)
    machine=macOS
    ;;
  *)
    error "This script isn't supported on your OS '${unameOut}'. Please, use Linux or macOS"
    exit 1
esac

if [ "${machine}" == "Linux" ];
then
  "${CURRENT_DIR}/install_deps_ubuntu.sh" "$@"
elif [ "${machine}" == "macOS" ]; then
  "${CURRENT_DIR}/install_deps_macOS.sh"
fi
