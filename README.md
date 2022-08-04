# SC-machine

[![CI](https://github.com/ostis-ai/sc-machine/actions/workflows/main.yml/badge.svg)](https://github.com/ostis-ai/sc-machine/actions/workflows/main.yml)
[![codecov](https://codecov.io/gh/ostis-ai/sc-machine/branch/main/graph/badge.svg?token=WU8O9Z1DNL)](https://codecov.io/gh/ostis-ai/sc-machine)
[![docker](https://img.shields.io/docker/v/ostis/sc-machine?arch=amd64&label=Docker&logo=Docker&sort=date)](https://hub.docker.com/r/ostis/sc-machine)

**SC-machine** is a software implementation of semantic network storage and processing of information in language
of universal knowledge representation SC-code, provided by the [**OSTIS Technology**](https://github.com/ostis-ai/ostis-standard). 
SC-machine is a **platform-independent graph database management system** to store, retrieve and run tasks (agents) 
on knowledge graphs. All declarative (data, data structures, documentation, tasks specification, etc.) and procedural 
(programs, modules, systems, communication between systems) knowledge represents on the same language, named SC-code. 
Nowadays sc-machine includes:
- semantic storage to store graph constructions, named sc-memory (graph database);
- event-based agents processing, named sc-event-manager (asynced program workflows handling);
- tools to communicate with storage:
   - *native* API on C++;
   - *network* API, provided by *sc-server* to communicate with storage by network with help of Websocket 
     and JSON languages;
   - *sc-builder* to load information from SCs files into storage (SCs-code is a one of external representation 
     languages of SC-code);

SC-machine provide complete and sufficient native and network APIs to create, retrieve, update and delete operations and
also supports event-driven communication with sc-memory and sc-agents, located in it.

Start use [OSTIS-web-platform](https://github.com/ostis-ai/ostis-web-platform) to integrate all the world solutions ***by 
the way of the same technology***, ***the same data representation and programming language*** and ***the same program 
software***!

# Documentation: 

- A part of sc-machine documentation, represented in md format: [GitHub Pages](https://ostis-ai.github.io/sc-machine).
  It serves as a guide for installation, development and a brief description of the project modules.
- SCn-code represented sc-machine documentation: [LaTeX documentation](https://github.com/ostis-ai/ostis-web-platform/blob/develop/docs/main.pdf).
  This documentation fully describes the sc-machine: its components, software interfaces for interacting with sc-memory, 
  the characteristics and design of sc-memory, as well as the rationale for the sc-machine tools and solutions used.

# Quick start
Sc-machine is a core of any ostis-system and a part of on of them, named **OSTIS Web platform**.
You can use our [OSTIS Web platform](https://github.com/ostis-ai/ostis-web-platform) project to get up and running quickly.
# Installation
## Docker
We're providing a Docker image for this project. Head to [Installing with Docker](https://ostis-ai.github.io/sc-machine/docker) 
section of our docs to try it out! We recommend to use this way for sc-machine deployment.
## Native
If you do not have the opportunity to deploy the system using docker, please, refer to the docs for your OS: 
[Build on Linux](https://ostis-ai.github.io/sc-machine/build/linux-build/) or [Build on macOS](https://ostis-ai.github.io/sc-machine/build/osx-build/).
Nowadays sc-machine isn't *natively* support on Windows.

# Usage
Typical usage case is to install platform using Docker or by building from sources, build knowledge base and start 
**sc-server** for other apps. By default, sc-server is runnable on host 127.0.0.1 and port 8090. Before sc-server 
running, sure, that all needed SCs and SCg sources is located in kb folder.

To run sc-server, you can do the following:
```sh
#build kb
python3 scripts/build_kb.py -c config/sc-machine.ini <path to kb folder with SCs and SCg sources or repo.path file>
#launch server
python3 scripts/run_sc_server.py -c config/sc-machine.ini
``` 
To get more information about these scripts use `-h` flags upon their launch.

## Config

This repository provides a default configuration for sc-machine. To customize *sc-machine* usage you can 
[create your own config file](https://ostis-ai.github.io/sc-machine/other/config).

# Report

*When error or problem occurred*, [write issue](https://github.com/ostis-ai/sc-machine/issues) to OSTIS-web-platform 
team.

##### *This repository continues the development of [this sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0.*
