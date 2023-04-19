#!/usr/bin/env bash

set -e

APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)
INTERACTIVE_MODE=""

while getopts 'c:b:' OPTION; do
  case "$OPTION" in
    c)
      CONFIG_PATH="$OPTARG"
      CONFIG_ABSOLUTE_PATH=$(cd "$(dirname "$CONFIG_PATH")" && pwd -P)/$(basename "$CONFIG_PATH")
      ;;
    b)
      KB_SRC_PATH="$OPTARG"
      python3 $APP_ROOT_PATH/scripts/build_kb.py $KB_SRC_PATH -c $CONFIG_ABSOLUTE_PATH
      ;;
    i)
      INTERACTIVE_MODE="-i"
      ;;
    h)
      echo "script usage: $(basename \$0) [-c CONFIG_PATH] [-b KB_SRC_PATH]" >&2
      exit 1
      ;;
  esac
done
shift "$(($OPTIND -1))"

if [ -z "$CONFIG_ABSOLUTE_PATH" ]; then
    echo "no config path is provided."
    echo "script usage: $(basename \$0) [-c CONFIG_PATH] [-b KB_SRC_PATH] [-i]" >&2
    exit 1
fi

$APP_ROOT_PATH/bin/sc-component-manager -c $CONFIG_ABSOLUTE_PATH $INTERACTIVE_MODE

