#!/usr/bin/env bash
set -eo pipefail

if [[ -z "${SC_MACHINE_PATH}" || -z "${BINARY_PATH}" || -z "${BUILD_PATH}" ]];
then
  source "$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)/set_vars.sh"
fi

function usage() {
  cat <<USAGE
Usage: $0 [--dev]

Options:
  --dev:          Installs dependencies required to compile sc-machine
USAGE
  exit 1
}

packagelist_runtime=(
  file
  curl
  libglib2.0-dev
  python3
  python3-pip
  python3-venv
)

packagelist_dev=(
  libglib2.0-dev
  make
  cmake
  ccache
  llvm
  gcc
  g++
  clang
  libclang-dev
  default-jre
  clang-format
  libwebsocketpp-dev
  nlohmann-json3-dev
  python3-dev
)

packages=() 
packages+=("${packagelist_runtime[@]}")

while [ "$1" != "" ]; do
  case $1 in
  --dev)
    packages+=("${packagelist_dev[@]}")
    ;;
  -h | --help)
    usage # show help
    ;;
  *)
    usage
    exit 1
    ;;
  esac
  shift 1 # remove the current value for `$1` and use the next
done

if ! command -v apt> /dev/null 2>&1;
then
  RED='\033[22;31m'
  NC='\033[0m' # No Color
  echo -e "${RED}[ERROR] Apt command not found. Debian-based distros are the only officially supported.
Please install the following packages by yourself:
  ${packages[*]}
At the end run the following script:
  ${SC_MACHINE_PATH}/scripts/install_deps_python.sh ${NC}"
  exit 1
fi

sudo apt-get update && sudo apt-get install -y --no-install-recommends software-properties-common
sudo add-apt-repository -y universe
sudo apt-get update

sudo apt-get install -y --no-install-recommends "${packages[@]}"
sudo apt autoremove

"${SC_MACHINE_PATH}/scripts/install_deps_python.sh"
