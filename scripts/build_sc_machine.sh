#!/usr/bin/env bash
set -eo pipefail

CURRENT_DIR=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)
source "${CURRENT_DIR}/formats.sh"

if [[ -z "${PROBLEM_SOLVER_PATH}" || -z "${BUILD_PATH}" || -z "${BINARY_PATH}" ]];
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
    -f, --force       full rebuild with the deleting of the $(relative "${BINARY_PATH}") and $(relative "${BUILD_PATH}") folders
    -t, --tests       build tests
    -r, --release     release mode
    -h, --help        display this help and exit
    ${additional_options}
USAGE
  exit 0
}

while [ "$1" != "" ]; do
	case $1 in
		"-f"|"--force" )
			build_force=1
			;;
		"-t"|"--tests" )
			build_tests=1
			;;
    "-r"|"--release" )
      release_mode=1
      ;;
    "--add-options" )
      shift 1
      additional_options=${1}
      ;;
	  "-h"|"--help" )
	    usage
      exit 0
			;;
    * )
      ;;
	esac
	shift 1
done

stage "Build sc-machine"

if ((build_force == 1));
then
  stage "Clear latest build"
  rm -rf "${BIN_PATH}"
  rm -rf "${BUILD_PATH}"
  find "${PROBLEM_SOLVER_PATH}" -type d -name generated -exec rm -rf {} +
fi

tests_appendix="-DSC_BUILD_TESTS=ON"
release_mode_appendix="-DCMAKE_BUILD_TYPE=Release"

cd "${PROBLEM_SOLVER_PATH}"
cmake -B "${BUILD_PATH}" "${PROBLEM_SOLVER_PATH}" ${build_tests:+${tests_appendix}} ${release_mode:+${release_mode_appendix}} "$@"
cmake --build "${BUILD_PATH}" -j"$(nproc)"

stage "SC-machine built successfully"
