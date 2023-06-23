#!/usr/bin/env bash
set -eo pipefail

if [[ -z "${SC_MACHINE_PATH}" || -z "${BINARY_PATH}" || -z "${BUILD_PATH}" ]];
then
  source "$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"/set_vars.sh
fi

function usage() {
  cat <<USAGE

  Usage: $0 [--dev]

  Options:
      --dev:          installs dependencies required to compile sc-machine
USAGE
  exit 1
}
sudo apt-get update && sudo apt-get install -y --no-install-recommends software-properties-common
sudo add-apt-repository -y universe
sudo apt-get update
packagelist_runtime=(
  file
  curl
  libglib2.0-dev
  libboost-system-dev
  libboost-filesystem-dev
  libboost-program-options-dev
  python3
  python3-pip
)

packagelist_dev=(
  libglib2.0-dev
  libboost-system-dev
  libboost-filesystem-dev
  libboost-program-options-dev
  make
  cmake
  ccache
  llvm
  gcc
  g++
  clang
  libclang-dev
  default-jre
  libcurl4-openssl-dev
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
  shift # remove the current value for `$1` and use the next
done

sudo apt-get install -y --no-install-recommends "${packages[@]}"
sudo apt autoremove

pip3 install wheel setuptools
pip3 install -r "${SC_MACHINE_PATH}/requirements.txt"
