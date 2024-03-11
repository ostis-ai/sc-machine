#!/usr/bin/env bash
set -eo pipefail

CURRENT_DIR=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)
source "${CURRENT_DIR}/../formats.sh"

function usage() {
  cat <<USAGE
Usage: $(basename "$0") [OPTION]...

Options:
  -p, --project-path Path to project CMakeLists.txt (required)
  -s, --source-path  Path to project cxx sources (required)
  -b, --build-path   Path to project build (required)
  -f, --force        Full rebuild with the deleting of build directory
  -t, --tests        Build in tests mode
  -r, --release      Build in release mode
  --cmake-arg        Add new argument into cmake build
  -h, --help         Display this help and exit
USAGE
  exit 0
}

outer_cmake_args=()
while [ "$1" != "" ];
do
  case $1 in
    "-p"|"--project-path" )
      shift 1
      CMAKE_PATH="${1}"
      ;;
    "-s"|"--source-path" )
      shift 1
      SOURCES_PATH="${1}"
      ;;
    "-b"|"--build-path" )
      shift 1
      BUILD_PATH="${1}"
      ;;
    "-f"|"--force" )
      build_force=1
      ;;
    "-t"|"--tests" )
      build_tests=1
      ;;
    "-r"|"--release" )
      build_release=1
      ;;
    "--cmake-arg" )
      shift 1
      outer_cmake_args+=("${1}")
      ;;
    "-h"|"--help" )
      usage
      exit 0
      ;;
    * )
      echo -e "$(basename "$0"): unknown flag $1"
      echo "Try '$0 -h' for help"
      exit 2
      ;;
  esac
  shift 1
done

if ((build_force == 1));
then
  stage "Clear latest build"
  rm -rf "${BUILD_PATH}"
  find "${SOURCES_PATH}" -type d -name generated -exec rm -rf {} +
fi

tests_mode="-DSC_BUILD_TESTS=ON"
release_mode="-DCMAKE_BUILD_TYPE=Release"

if [[ -z "${CMAKE_PATH}" || -z "${SOURCES_PATH}" || -z "${BUILD_PATH}" ]];
then
  usage
fi

cd "${CMAKE_PATH}"
cmake -B "${BUILD_PATH}" "${CMAKE_PATH}" ${build_tests:+${tests_mode}} ${build_release:+${release_mode}} "${outer_cmake_args[@]}"
cmake --build "${BUILD_PATH}" -j"$(nproc)"
