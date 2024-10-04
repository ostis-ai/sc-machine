#!/usr/bin/env bash
set -eo pipefail
CURRENT_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"
source "${CURRENT_DIR}/../formats.sh"

if ! which java &> /dev/null; then
    warning "Java is not installed. To run this you need java with version 8 or higher installed."
    exit 1
fi

java -jar "$CURRENT_DIR/migrate_to_new_agent_api.jar" "$@"
