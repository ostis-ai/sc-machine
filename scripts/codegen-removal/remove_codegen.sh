#!/usr/bin/env bash
set -eo pipefail
CURRENT_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"

java -jar "$CURRENT_DIR/codegen-removal.jar" "$@"
