#!/usr/bin/env bash
set -eo pipefail

SCRIPTS_PATH="$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"

brew install glib temurin pkgconfig ninja ccache websocketpp nlohmann-json libxml2 googletest google-benchmark

# TODO(NikitaZotov): Remove asio version 1.30.2 pinning below.
# Required because websocketpp 0.8.2 is incompatible with newer asio.
# Remove after updating websocketpp to a version including the fix from PR https://github.com/zaphoyd/websocketpp/pull/1164 or later.
brew uninstall --ignore-dependencies asio
curl -O https://raw.githubusercontent.com/Homebrew/homebrew-core/502489d3a4c1ca0a3854830eb5da2327b6feb54d/Formula/a/asio.rb
brew install asio.rb
brew pin asio

"${SCRIPTS_PATH}/install_deps_python.sh"
