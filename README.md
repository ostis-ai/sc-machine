# sc-machine

[![CI](https://github.com/ostis-ai/sc-machine/actions/workflows/test_conan.yml/badge.svg)](https://github.com/ostis-ai/sc-machine/actions/workflows/test_conan.yml)
[![CI](https://github.com/ostis-ai/sc-machine/actions/workflows/test_native.yml/badge.svg)](https://github.com/ostis-ai/sc-machine/actions/workflows/test_native.yml)
[![codecov](https://codecov.io/gh/ostis-ai/sc-machine/branch/main/graph/badge.svg?token=WU8O9Z1DNL)](https://codecov.io/gh/ostis-ai/sc-machine)
[![license](https://img.shields.io/badge/License-MIT-yellow.svg)](COPYING.MIT)
[![docker](https://img.shields.io/docker/v/ostis/sc-machine?arch=amd64&label=Docker&logo=Docker&sort=semver)](https://hub.docker.com/r/ostis/sc-machine)

[![Quick Start](https://img.shields.io/badge/-Quick%20Start-black?style=for-the-badge&logo=rocket)](https://ostis-ai.github.io/sc-machine/quick_start)
[![Docs](https://img.shields.io/badge/Docs-gray?style=for-the-badge&logo=read-the-docs)](https://ostis-ai.github.io/sc-machine)
[![Community](https://img.shields.io/badge/-Community-teal?style=for-the-badge&logo=matrix)](https://app.element.io/index.html#/room/#ostis_tech_support:matrix.org)

Short version: **sc-machine** is a software package that emulates **semantic computer behavior** by storing and processing knowledge in a semantic network. It uses **agent-based approach** to process knowledge graphs.

From the perspective of graph databases, sc-machine is a **graph database management system** designed to store / retrieve knowledge graphs and run tasks (agents) on them.

sc-machine stores and processes knowledge graphs represented in the SC-code (language of the universal knowledge representation). Theoretical basis of sc-machine is provided by the [**OSTIS Technology**](https://github.com/ostis-ai).

Both declarative (data, data structures, documentation, problems specification, etc.) and procedural (agents, programs, modules, systems, communication between systems) knowledge is represented using that same language.

Semantic network storage allows integrating problem solutions from different subject domains **by using the same**:

- **technology**
- **programming and data representation language**
- **software stack**.

## Getting started

To get started, check out our [quick start guide](https://ostis-ai.github.io/sc-machine/quick_start).


## This project contains:

- `sc-memory` - semantic storage to store graph constructions;
- `sc-event-manager` - event-based processing (async program workflow handling);
- tools to communicate with `sc-memory`:
  - native C++ API;
  - `sc-server` - a Websocket server that provides a network API to communicate with storage using JSON;
  - `sc-builder` loads information from SCs files into the storage (SCs-code is one of the external representation languages of the SC-code).

## Documentation

- A brief user manual and developer docs are hosted on our [GitHub Pages](https://ostis-ai.github.io/sc-machine).
  - <details>
      <summary>Build documentation locally</summary>

    ```sh
    # make sure you're using python 3.12
    pip3 install mkdocs mkdocs-material mkdocs-i18n
    mkdocs serve
    # and open http://127.0.0.1:8000/ in your browser
    ```
    </details>
- Full documentation, including:

  - core concepts
  - rationale behind the sc-machine
  - system design
  - software interfaces
  - more rigorous theoretical calculations

  is redistributed in a form of the SCn-TeX document [compiled in PDF format](https://github.com/ostis-ai/ostis-web-platform/blob/develop/docs/main.pdf).

  **Alternatively** you can build sc-machine documentation yourself. To do that refer to the [scn-latex-plugin](https://github.com/ostis-ai/scn-latex-plugin) documentation.

## Feedback

Contributions, bug reports and feature requests are welcome!
Feel free to check our [issues page](https://github.com/ostis-ai/sc-machine/issues) and file a new issue (or comment in existing ones).

## License

Distributed under the MIT License. Check [COPYING.MIT](COPYING.MIT) for more information.

##### _This repository continues the development of [ostis-dev/sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0._
