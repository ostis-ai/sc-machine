#!/bin/bash
set -eo pipefail

if [[ -z ${APP_ROOT_PATH+1} ]];
then
  source "$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"/set_vars.sh
fi

relative()
{
  realpath --relative-to="$(pwd)" "$1"
}

while [ "$1" != "" ]; do
	case $1 in
		"-f"|"--force" )
			build_force=1
			;;
		"-t"|"--tests" )
			build_tests=1
			;;
    "-r" | "--release" )
      release_mode=1
      ;;
	  "-h"|"--help")
      echo "Usage: $(basename "$0") [OPTION]..."
      echo
      echo "Options:"
      echo "  -f, --force                 full rebuild with the deleting of the $(relative "${APP_ROOT_PATH}/bin") and $(relative "${APP_ROOT_PATH}/build") folders"
      echo "  -t, --tests                 build tests"
      echo "  -r, --release               release mode"
      echo "  -h, --help                  display this help and exit"
      exit 0
			;;
    *)
      echo -e "$(basename "$0"): unknown flag $1"
      echo "Try '$(basename "$0") -h' for help"
      exit 1
	esac
	shift
done

printf "Build sc-machine"

if ((build_force == 1));
then
  printf "Clear latest build"
	rm -rf "${APP_ROOT_PATH:?}/bin"
	rm -rf "${APP_ROOT_PATH}/build"
	find "${APP_ROOT_PATH}" -type d -name generated -exec rm -rf {} +
fi

tests_appendix="-DSC_BUILD_TESTS=ON"
release_mode_appendix="-DCMAKE_BUILD_TYPE=Release"

cmake -B "${APP_ROOT_PATH}/build" "${APP_ROOT_PATH}" ${build_tests:+${tests_appendix}} ${release_mode:+${release_mode_appendix}}
cmake --build "${APP_ROOT_PATH}/build" -j$(nproc)

cd "${WORKING_PATH}"

printf "SC-machine built"