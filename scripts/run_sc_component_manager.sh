#!/usr/bin/env bash

set -e

APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)

while getopts 'c:b:' OPTION; do
  case "$OPTION" in
    c)
      config_path="$OPTARG"
      CONFIG_ABSOLUTE_PATH=$(cd "$(dirname "$config_path")" && pwd -P)/$(basename "$config_path")
      ;;
    b)
      kb_src_path="$OPTARG"
      python3 $APP_ROOT_PATH/scripts/build_kb.py $kb_src_path -c $CONFIG_ABSOLUTE_PATH
      ;;
    h)
      echo "script usage: $(basename \$0) [-c config_path] [-b kb_src_path]" >&2
      exit 1
      ;;
  esac
done
shift "$(($OPTIND -1))"

if [ -z "$CONFIG_ABSOLUTE_PATH" ]; then
    echo "no config path is provided."
    echo "script usage: $(basename \$0) [-c config_path] [-b kb_src_path]" >&2
    exit 1
fi

$APP_ROOT_PATH/bin/sc-component-manager -c $CONFIG_ABSOLUTE_PATH -i

