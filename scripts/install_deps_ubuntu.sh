#!/bin/bash
APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)

function usage() {
    cat <<USAGE

    Usage: $0 [--rocksdb] [--dev]

    Options:
        --rocksdb:      installs deps required for RocksDB storage backend
        --dev:          installs dependencies required to compile sc-machine
USAGE
    exit 1
}

sudo apt-get update
packagelist_runtime=(
    libqt5network5
    libcurl4
    libglib2.0-dev
    libboost-system-dev
    libboost-filesystem-dev
    libboost-program-options-dev
    libboost-python-dev
    python3
    python3-pip
)

packagelist_dev=(
    qtbase5-dev
    libglib2.0-dev
    libboost-system-dev
    libboost-filesystem-dev
    libboost-program-options-dev
    libboost-python-dev
    make
    cmake
    ccache
    llvm
    gcc
    g++
    libclang-dev
    default-jre
    libcurl4-openssl-dev
    clang-format
    python3-dev
    ccache
)

packagelist_rocksdb=(
librocksdb-dev
)

packages=() 
packages+=(${packagelist_runtime[@]})

while [ "$1" != "" ]; do
    case $1 in
    --dev)
       packages+=(${packagelist_dev[@]})
        ;;
    --rocksdb)
       packages+=($packagelist_rocksdb[@]) 
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

sudo apt-get install -y --no-install-recommends ${packages[@]}

pip3 install wheel setuptools
pip3 install -r ${APP_ROOT_PATH}/requirements.txt
