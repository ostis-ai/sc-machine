#!/usr/bin/env bash
APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)

"${APP_ROOT_PATH}"/bin/sc-builder -i "${APP_ROOT_PATH}"/sc-tools/sc-builder/tests/kb -o "${APP_ROOT_PATH}"/bin/sc-builder-test-repo --clear -f
"${APP_ROOT_PATH}"/bin/sc-builder -i "${APP_ROOT_PATH}"/sc-tools/sc-server/tests/kb -o "${APP_ROOT_PATH}"/bin/sc-server-test-repo --clear -f

cd "${APP_ROOT_PATH}"/build && ctest -C Debug -V
