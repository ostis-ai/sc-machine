#!/bin/bash
APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)
cmake -B "${APP_ROOT_PATH}"/build "${APP_ROOT_PATH}" "$@"
cmake --build "${APP_ROOT_PATH}"/build -j$(nproc)
