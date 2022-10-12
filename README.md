# SC-machine

[![CI](https://github.com/ostis-ai/sc-machine/actions/workflows/main.yml/badge.svg)](https://github.com/ostis-ai/sc-machine/actions/workflows/main.yml)
[![codecov](https://codecov.io/gh/ostis-ai/sc-machine/branch/main/graph/badge.svg?token=WU8O9Z1DNL)](https://codecov.io/gh/ostis-ai/sc-machine)
[![license](https://img.shields.io/badge/License-MIT-yellow.svg)](COPYING.MIT)
[![docker](https://img.shields.io/docker/v/ostis/sc-machine?arch=amd64&label=Docker&logo=Docker&sort=date)](https://hub.docker.com/r/ostis/sc-machine)

Short version: **SC-machine** is a software package that emulates semantic computer behaviour. It uses agent-based approach to to process knowledge graphs.

SC-machine stores and processes knowledge graphs represented in the SC-code (language of the universal knowledge representation). Theoretical basis of SC-machine is provided by the [**OSTIS Technology**](https://github.com/ostis-ai/ostis-project).

SC-machine allows integrating problem solutions from different subject domains **by using the same**:

- **technology**
- **programming and data representation language**
- **software stack**.

This project contains:

- `sc-memory` - semantic storage to store graph constructions;
- `sc-event-manager` - event-based processing (async program workflow handling);
- tools to communicate with `sc-memory`:
  - native C++ API;
  - `sc-server` - a Websocket server that provides a network API to communicate with storage using JSON;
  - `sc-builder` loads information from SCs files into the storage (SCs-code is one of the external representation languages of the SC-code).

<details>
   <summary>More info</summary>

SC-machine is a **platform-independent graph database management system** that can store / retrieve knowledge graphs and run tasks (agents) on them.

Both declarative (data, data structures, documentation, tasks specification, etc.) and procedural
(programs, modules, systems, communication between systems) knowledge is represented using the same language: the SC-code.

</details>

## Documentation

- A brief user manual and developer docs are hosted on our [GitHub Pages](https://ostis-ai.github.io/sc-machine).
- Full documentation, including:

  - core concepts
  - rationale behind the SC-machine
  - system design
  - software interfaces

  is redistributed in a form of the [SCn-TeX document](https://github.com/ostis-ai/ostis-web-platform/blob/develop/docs/main.pdf).

## Quick start

SC-machine is a core of any OSTIS-system, so you can use a reference system named [OSTIS Web platform](https://github.com/ostis-ai/ostis-web-platform) to get it up and running quickly.

## Installation

- Docker:
  We provide a Docker image for this project. Head to [Installing with Docker](https://ostis-ai.github.io/sc-machine/docker) to learn more. It's the recommended way to deploy the SC-machine.
- Native:
  If you do not have the option to deploy the system using Docker, please refer to the docs for your OS:
  [Build on Linux](https://ostis-ai.github.io/sc-machine/build/linux-build/) or [Build on macOS](https://ostis-ai.github.io/sc-machine/build/osx-build/) respectively.

  Note: currently, the SC-machine isn't _natively_ supported on Windows.

## Usage

- Docker

  ```sh
  # build kb
  docker run --rm -it -v <full path to kb sources>:/kb -v sc-machine_kb:/kb.bin ostis/sc-machine:latest build /kb
  # run sc-server
  docker run --rm -it -v sc-machine_kb:/kb.bin -p 8090:8090 ostis/sc-machine:latest serve
  ```

- Native

  ```sh
  #build kb
  python3 scripts/build_kb.py -c config/sc-machine.ini <path to kb folder with SCs and SCg sources (or path to repo.path file)>
  #launch sc-server
  ./bin/sc-server -c config/sc-machine.ini
  ```

Most of these commands have a help page bundled inside, so if you have any questions or want to customize the command behavior, use `--help` flag to learn more about them.

## Config

This repository provides a default configuration for the SC-machine. To customize the _SC-machine_ to suit your needs you can [create your own config file](https://ostis-ai.github.io/sc-machine/other/config).

## Feedback

Contributions, bug reports and feature requests are welcome!
Feel free to check our [issues page](https://github.com/ostis-ai/sc-machine/issues) and file a new issue (or comment in existing ones).

## License

Distributed under the MIT License. Check [COPYING.MIT](COPYING.MIT) for more information.

##### _This repository continues the development of [ostis-dev/sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0._
