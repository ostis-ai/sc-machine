#!/usr/bin/env bash
set -eo pipefail

SCRIPTS_PATH="$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"

brew install glib temurin pkgconfig cmake ninja ccache websocketpp nlohmann-json libxml2 googletest google-benchmark

curl -O https://raw.githubusercontent.com/Homebrew/homebrew-core/502489d3a4c1ca0a3854830eb5da2327b6feb54d/Formula/a/asio.rb
brew install asio.rb

"${SCRIPTS_PATH}/install_deps_python.sh"
