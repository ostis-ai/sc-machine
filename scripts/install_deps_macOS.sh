#!/usr/bin/env bash
set -eo pipefail

SCRIPTS_PATH="$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"

brew install glib temurin pkgconfig cmake ninja ccache asio websocketpp nlohmann-json libxml2 googletest google-benchmark

"${SCRIPTS_PATH}/install_deps_python.sh"
