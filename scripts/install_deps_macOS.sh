#!/usr/bin/env bash
set -eo pipefail

if [ -z "${SC_MACHINE_PATH}" ];
then
  source "$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)/set_vars.sh"
fi

brew install boost glib llvm@14 antlr antlr4-cpp-runtime temurin pkgconfig cmake ninja ccache websocketpp nlohmann-json

"${SC_MACHINE_PATH}/scripts/install_deps_python.sh"
