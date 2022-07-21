# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.238.0/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version (use Debian 11, Ubuntu 18.04/22.04 on local arm64/Apple Silicon): debian-11, debian-10, ubuntu-22.04, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="bullseye"
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}
ENV CCACHE_DIR=/ccache

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"
# tmp paths are needed to recreate the project structure (install_deps_ubuntu depends on requirements.txt)
COPY scripts/install_deps_ubuntu.sh /tmp/sc-machine/scripts/install_deps_ubuntu.sh
COPY requirements.txt /tmp/sc-machine/requirements.txt

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && /tmp/sc-machine/scripts/install_deps_ubuntu.sh --dev
