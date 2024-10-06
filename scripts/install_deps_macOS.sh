#!/usr/bin/env bash
set -eo pipefail

if [ -z "${SC_MACHINE_PATH}" ];
then
  source "$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)/set_vars.sh"
fi

brew install glib antlr antlr4-cpp-runtime temurin pkgconfig cmake ninja ccache asio websocketpp nlohmann-json libxml2

"${SC_MACHINE_PATH}/scripts/install_deps_python.sh"
