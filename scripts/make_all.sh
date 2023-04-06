#!/bin/bash
APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)
COMPONENT_MANAGER_SUPPORT=False

while getopts ":m" flag; do
    case "${flag}" in
        m)
            COMPONENT_MANAGER_SUPPORT=True
            ;;
        *)
            return
            ;;
    esac
done
shift $((OPTIND-1))

cmake -B "${APP_ROOT_PATH}"/build "${APP_ROOT_PATH}" "$@" "-DSC_COMPONENT_MANAGER:INTERNAL=$COMPONENT_MANAGER_SUPPORT"
cmake --build "${APP_ROOT_PATH}"/build -j$(nproc)
