# SC-machine

[![CI](https://github.com/ostis-ai/sc-machine/actions/workflows/main.yml/badge.svg)](https://github.com/ostis-ai/sc-machine/actions/workflows/main.yml)
[![codecov](https://codecov.io/gh/ostis-ai/sc-machine/branch/main/graph/badge.svg?token=WU8O9Z1DNL)](https://codecov.io/gh/ostis-ai/sc-machine)
[![docker](https://img.shields.io/docker/v/ostis/sc-machine?arch=amd64&label=Docker&logo=Docker&sort=date)](https://hub.docker.com/r/ostis/sc-machine)

# Documentation: [GitHub Pages](https://ostis-ai.github.io/sc-machine)

# Quick start
You can use our [OSTIS Web platform](https://github.com/ostis-ai/ostis-web-platform) project to get up and running quickly.
# Installation
## Docker
We're providing a Docker image for this project. Head to [Installing with Docker](https://ostis-ai.github.io/sc-machine/docker) section of our docs to try it out!
## Native
Please refer to the docs for your OS: [Build on Linux](https://ostis-ai.github.io/sc-machine/build/linux-build/) or [Build on macOS](https://ostis-ai.github.io/sc-machine/build/osx-build/)

# Usage
Typical usage case is to install platform using Docker or by building from sources, build knowledge base and start `sc-server` for other apps.

For that, you can do the following:
```sh
#build kb
python3 scripts/build_kb.py -c config/sc-machine.ini <path to KB or repo.path file>
#launch server
python3 scripts/run_sc_server.py -c config/sc-machine.ini
``` 
To get more information about these scripts use `-h` flags upon their launch.

## Config

This repository provides a default configuration for sc-machine. To customize *sc-machine* usage you can [create your own config file](https://ostis-ai.github.io/sc-machine/other/config).


##### *This repository continues the development of [this sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0.*
