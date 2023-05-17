# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Breaking changes
- Now we use C++17 instead of C++14

### Added

- Get replacements in template result by variable address
- Configs for file memory string sections
- Divide strings file into more small files with fixed size by requested configs
- Configuration of docker-compose.yml using .env file
- Script run_tests.sh
- Tests for fs-memory
- Intersect/unite search by strings in fs-memory
- Search strings and link hashes by terms in fs-memory
- File memory on file system, save strings in file system disks
- Configure events and agents threads
- Provide loop-ranged API method for sc-template search ([78](https://github.com/ostis-ai/sc-machine/issues/78))
- Sc-template cycle preventing search
- Sc-template equal triples search
- Sc-template search API to get and use constructions step by step
- Sc-template search API to custom filter constructions with step by step search
- Sc-template search API with request commands (continue search, stop search and error during search)
- Cover all sc-template search and gen API with tests
- Order triples by type and dependencies with other triples
- Count input/output edges in elements during edges adding
- Order triples by input/output edges count for triple items
- Determined sc-template depth-first search
- Sc-template safe API without exceptions handling
- Extend create elements by SCs in `sc-server` with outputStructure field
- Write docs for system identifier set/get/find/resolve sc-memory API
- Extend set/get/find/resolve sc-memory API with out system identifier fiver parameters
- Insert sc-keynodes in global knowledge base during sc-memory initiation
- Save sc-memory by period in sc-server
- Dump statistics by period in sc-server
- Erase sc-elements agent
- Transfer init memory generated structure to modules initialization
- Possibility to skip keys defined in configs
- Pre-commit code style checks
- Add reversed edges according to standart
- Add `![]!` parsing to scs parser
- Insert sc-keynodes and their system identifiers in global knowledge base during sc-memory initiation
- Extend create elements by SCs in `sc-server` with outputStructure field
- Write docs for system identifier set/get/find/resolve sc-memory API
- Extend set/get/find/resolve sc-memory API with out system identifier fiver parameters
- CI for documentation build
- Fix memory clear parameter reading from config file
- Add output structure builder feature to config
- Add validation of system identifier
- Append generated elements by `SCs-helper` into requested output structure
- Oriented sets representation (<a, ..., b>) in SCs
- Build docs in SCn format

### Changed

- Unite ScTemplateGenResult and ScTemplateSearchResultItem into ScTemplateResultItem
- Remove list to array translations in C API for fs-memory
- Upload text and binary files in realtime by get API
- Remove string lengths recount in fs-memory
- Optimize int to string translation in fs-memory
- Use iterators instead of sc-template in `getNextFromSet` util
- Generalized docker_entrypoint.sh, this script can be used by external projects now
- Now in tests all agents works in single thread
- Replace asserts in sc-template search and gen API by exceptions throwing
- `ScsLoader::loadScsFile` return bool instead void
- Delete elements endpoint uses erase elements agent
- Update garbage deletion agent logic, add check if element belongs to init memory structure
- Make scsi pre-processor migrations scrip
- Move to C++17
- Remove 1 minute assert in ScWait
- Add language parameter for sc2scs-json-translator to get elements identifiers
- Append to sc2scs-json elements main/system identifiers
- Replace row strings by nlohmann-json in sc2scs-json-translator

### Fixed

- Tests for template build and gen with params
- Aliases checking in build by scs
- Not use system identifiers of vars in sc-template-build
- Sc-template search by empty template ([65](https://github.com/ostis-ai/sc-machine/issues/65)))
- Segfaults in test sc-client
- Creating sc-keynodes without defining type. Now they will be created with const node type.
- Converting to string params from configs
- Fix hashes size in fs-storage ([45](https://github.com/ostis-ai/ostis-web-platform/issues/45))
- GetLinkContent skip whitespaces in strings
- No inserting system identifiers of keynodes to output structure ([223](https://github.com/ostis-ai/sc-machine/issues/223))
- `SCs-helper` doesn't return generated identifier links ([207](https://github.com/ostis-ai/sc-machine/issues/207))
- No json string message parsing in `sc-server` ([190](https://github.com/ostis-ai/sc-machine/issues/190))
- `SCs-parser` create structure twice
- `SCs-parser` doesn't unite all generated elements into structure ([205](https://github.com/ostis-ai/sc-machine/issues/205))
- Link deletion ([193](https://github.com/ostis-ai/sc-machine/issues/193)
- Add curl to installation script
- Merging identifiers in sc-dictionary ([192](https://github.com/ostis-ai/sc-machine/issues/192))
- Dependence order in var template search triples ([186](https://github.com/ostis-ai/sc-machine/issues/186))

### Removed

- Remove rocksdb
- Garbage deletion agent

## [0.7.0-Rebirth] - 12.10.2022

### Breaking changes

- Binaries are built in the `bin` folder located near the CMake build tree. This will affect any project using sc-machine as a CMake subproject.
- Add `;` after `SC_LOG_INFO`, `SC_LOG_DEBUG`, `SC_LOG_WARNING` and `SC_LOG_ERROR` calls in projects that use sc-machine.
- Add `;` after `SC_ASSERT` calls in projects that use sc-machine.
- Remind that all binary content are stored in sc-fs-storage as base64 string.
- sctp-server was removed, move to sc-server usage.
- SC-machine doesn't support ubuntu versions lower than 20.04.

### Added

- Implement sc-link content removing from sc-storage
- Add command to find links contents by content substring into sc-server
- Implement find links contents by content substring into sc-server
- CI for docker build and run
- CI for ubuntu-22.04 and latest macOS
- Add ScExec class to execute system commands
- Test for utils for work with actions and their results
- Handle and save sc-server subcommands errors
- Add sc-server healthcheck
- Add tests for sc-search agents in sc-kpm
- Write sc-machine idea into readme
- Add create elements by scs-helper through sc-server
- Add CI workflow for sanitizers
- Add find links by substring in sc-server
- Initial development container support
- You can now run sc-machine in Docker
- Unite configuration for sc-server and sc-builder
- Write docs in scn-latex for sc-server
- Implement sc-memory configurator
- Implement sc-options for sc-server and sc-builder usages
- Implement sc-server on C++
- Add string API for set and get link content
- Add opportunity to search sc-links in `sc-dictionary` by content substr
- Excludes for files and folders in repo.path
- Automatic usage of ccache to speed up builds
- Add CI for `rocksdb` and `sc-dictionary`
- Implement `sc-dictionary`. Add opportunity to switch `rocksdb` and `sc-dictionary`
- Wrap and separate allocating, assertion, notification and atomic lock free procedures
- Add tests module for sc-agents-utils
- Add opportunity to configure sc-machine version from CMakeLists.txt
- Add scripts to build kb, run sctp-server and run sc-server
- New flag options for build_kb
- Add default python modules paths to python initialization
- United config file

### Changed

- Fix sc-server json casts to strings
- Check sc-server run in tests
- Flag --tests for sc-server to run and quick stop it
- Fix glib casts on ubuntu-20.04 and macOS
- Store binary content as base64 string
- Update action utils to use the new logic for waiting for action results
- Fix logic of waiting for action results
- Entrypoint of Docker image is now easier to work with
- Update commit badges
- Up codecov target from 40% to 75%
- Up codecov level from 62% to 78%
- Separate ci workflow: check pr-commit, codestyle and tests
- Revamped README.md
- Unify sc-builder and sc-server config and params usage
- Change sc-machine config file structure
- Fix gwf2scs-translator. Add opportunity to parse russian identifiers
- Simplify main CMakeLists.txt
- Add tools macros for main CMakeLists.txt
- Separate dependencies specifying from main CMakeLists.txt
- Correct typos
- Fix warnings in sc-memory modules
- Fix scs-grammar. Add opportunity to specificate a structure into this structure itself

### Removed

- Remove find by substring logic from sc-dictionary
- Remove legacy gwf-translator for 0.3.0 gwf sources from sc-builder
- Remove legacy glib-based tests
- Remove boost usage for options parsing in sc-server and sc-builder
- Unlink boost-python-lib from sc-machine
- Remove mkdocs for sc-server
- Remove sc-python-interpreter support
- Remove sc-python-module support
- Remove python sc-server
- Move logic rule utils and keynodes to [ostis-inference](https://github.com/ostis-apps/ostis-inference)
- Remove sctp-server
- Remove scp-interpreter

## [0.6.1] - 27.04.2022

### Added

- Search by template with params in C++ API
- Sanitizers
- Benchmarks
- Clang code formatting
- Common utils for C++ API

### Changed

- Migrate test system to `gtest`
- Update macOS install_deps script
- Multithreaded build process used by default
- Move changelog to [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) format

### Removed

- Moved React web interface in a separate [new repository.](https://github.com/ostis-ai/react-sc-web-ui-template)
- Remove optional search triples support

## v0.6.0

- **Build system**:

  - Add support of OSX platform
  - Fix compilation with `clang`
  - Disable `SCP` and `SCTP` compilation by default
  - Support of C++14
  - Use Ubuntu 18.04 on Travis CI
  - Upgrade `libclang` to version 7

- **Tools**:

  - Support custom extensions loading in `sc-builder`
  - Add `sc-server` application to run claen `sc-memory` with extensions
  - Refactored code generation tool
  - Refactored knowledge base tool

- **Core**:

  - Support list of enabled extensions
  - Support all possible combinations of `sc_iterator3`
  - Support variable `sc-links`
  - Fix bug with repo directory write attributes
  - Fix issue with extension loading on windows
  - Fix issue with empty content of `sc-link`
  - Refactored file memory to use rocksdb

- **C++**:

  - Implement optional search triples support
  - Fix random crash of unittest
  - Improve test system
  - Replace `antlr3` with `antlr4` for `SCs-text` parsing
  - Support of `SCs-text` aliases
  - Support of 5 and 6 levels of `SCs-text`
  - Refactored test system
  - Refactored API of `ScLink` and `ScStream`
  - Add generation of structure from `SCs-text`

- **Python**:

  - Refactored test system
  - Support SCs-templates in WebSocket protocol
  - Add support of template building from `SCs-code` string
  - Add `Aliases` method to `ScTemplateSearchResult` and `ScTemplateGenResult`
  - Fix issue with float/double values support
  - Add support of logs inside python modules
  - Support `ActionAgent` states

- **KPM**:

  - **web**:
    - Implement `http` module to access memory via Web interface
    - Implement web browser interface to access `sc-memory`
  - Remove of `sc-merge` module

- **Documentation**:
  - Improve `SCs-code` documentation. Add levels `5` and `6`

## v0.5.0

- **Build system**:

  - Add Visual Studio 2017 support

- **Core**:

  - Fix bug with repository save (case when it output directory doesn't exist)

- **C++**:

  - Fix bugs in python GIL work
  - Fix python threading
  - Fix problem with twicely pass of edge in template search
  - Fix problem with stack overflow on large template search
  - Fix some python wrapping functions
  - Improve `ScType` class
  - Add types support in SCs-parser
  - Add keynode cache
  - Add `ScTemplate` build from SCs-text
  - Add support of pending events. Now events emit after whole template generated

- **Python**:

  - Add `ScAgent` class
  - Improve custom main cycle in `ScModule`
  - Improve python testing
  - Implement `ScHelper` class

- **Tools**:

  - Fix error handling in sc-builder
  - Fix `show-filenames` parameter processing in sc-builder

- **Documentation**
  - Update documentation theme
  - Improve python library documentation
  - Support `SCs-syntax` highlight in documentation

## v0.4.1

- Add `ScSet` and `ScRelationSet` in python common library
- Add `ScPythonService` into C++ (allow to implement threaded python services)
- Add `GetType` and `AsBinary` functions into Python for a link content
- Add colors to python output
- Add signal handler to handle `Ctrl + C`
- Fix bug with python modules search path initialization
- Fix bug with python threading
- Fix bug with builder run
- Fix bug with `ScLock`
- Fix bug with Python interpreter shutdown
- Fix bug with `int8_t` type support in python bindings
- Fix bug with template generation, when pass parameter to triple with type that has no constancy flag
- Fix errors catch during collect python modules
- Refactoring of C++ Python bridge

## v0.4.0

- Add events support in Python scripts
- Improve Python scripts run
- Add common libraries for a Python
- Fix memory leaks in core
- Improve test logging
- Improve work with memory buffers
- Cleanup API
- Add support of python 3 code
- Add `ScLockScope` class
- Implement multithreaded python script run
- Pass `cpp_bridge` into python script
- Extract `ScWaitEvent` as a separated class from `ScWait`
- Some fixes in template search
- Make log thread safe
- Implement `ScLink` wrapper class
- Add `ScBase64` encode/decode functions
- Add common templates generation (see `sc_common_templ.hpp`)

## v0.3.1

- Fix templates search error (problems was with triples search order)
- Implement logging for load extension module problems
- Remove `nl`, `iot` modules from kpm, because they are project specific (moved to a separate repo)
- Add `ForEach` function into template search result
- Add `ForEachIter` functions into `ScMemoryContext`
- Add `ScSet` class
- Add support of http requests

## v0.3.0

- Implement search for elements, that has no constancy flag in a type
- Fix template search error (#251)
- Fix problem with `SC_ASSERT` compilation in release mode (#249)
- Implement SCs-text parser (levels 1 - 4)
- Agent `AApiAiParseUserTextAgent` can run commands
- Implement common function to work with commands in knowledge base
- Add support of keynode types in codegenerator. Now you can specify type of keynode that would be used to create it: `ForceCreate(ScType::Node...)`
- Rename module funcitons from `initialize`, `shutdown`, `load_priority` to `sc_module_initialize`, `sc_module_shutdown`, `sc_module_load_priority`
- Update code to codestyle. Most functions was renamed from `functionName` to `FunctionName`. Iterator `value` function replaced with `Get`.
- Implement support of result codes for action agents
- Improve unit test system
- Add [MkDocs](http://www.mkdocs.org/) documentation generator support
- Add `clang-format` config
- Add codestyle document
- Add documentation for `C++` API
- Implement multithreaded emit of sc-events
- Implement ref counter for sc-elements
- Cover whole functionality of `C++` API by unit tests
- Implement `C++` API

[unreleased]: https://github.com/ostis-ai/sc-machine/compare/0.6.1...HEAD
[0.6.1]: https://github.com/ostis-ai/sc-machine/compare/0.6.0...0.6.1
