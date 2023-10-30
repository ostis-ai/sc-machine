#!/usr/bin/env bash
set -eo pipefail

CURRENT_DIR=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)
source "${CURRENT_DIR}/formats.sh"

if [[ -z "${ROOT_CMAKE_PATH}" || -z "${PROBLEM_SOLVER_PATH}" || -z "${BUILD_PATH}" || -z "${BINARY_PATH}" ]];
then
  source "${CURRENT_DIR}/set_vars.sh"
fi

relative()
{
  realpath --relative-to="$(pwd)" "$1"
}

function usage() {
  cat <<USAGE
Usage: $(basename "$0") [OPTION]...

Options:
  -f, --force        full rebuild with the deleting of the $(relative "${BINARY_PATH}") and $(relative "${BUILD_PATH}") folders
  -t, --tests        build in tests mode
  -r, --release      build in release mode
  --cmake-arg        add new argument into cmake build
  -h, --help         display this help and exit
USAGE
  exit 0
}

outer_cmake_args=()
while [ "$1" != "" ]; do
	case $1 in
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
  rm -rf "${BINARY_PATH}"
  rm -rf "${BUILD_PATH}"
  find "${PROBLEM_SOLVER_PATH}" -type d -name generated -exec rm -rf {} +
fi

tests_mode="-DSC_BUILD_TESTS=ON"
release_mode="-DCMAKE_BUILD_TYPE=Release"

stage "Build sc-machine"

cd "${ROOT_CMAKE_PATH}"
cmake -B "${BUILD_PATH}" "${ROOT_CMAKE_PATH}" ${build_tests:+${tests_mode}} ${build_release:+${release_mode}} "${outer_cmake_args[@]}"
cmake --build "${BUILD_PATH}" -j"$(nproc)"

stage "SC-machine built successfully"
