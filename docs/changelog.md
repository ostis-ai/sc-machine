# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Breaking changes

- All methods in C++ API for managing sc-memory throws exceptions if passed params are not valid or memory state is not valid
- All methods in C API for managing sc-memory returns error codes if passed params are not valid or memory state is not valid
- Removed deprecated sc-utils in 0.6.0, 0.7.0 and 0.8.0
- Removed deprecated make_all.sh

### Added

- Divide params errors handling and memory state handling
- Provide errors and exceptions information into C and C++ sc-memory API
- Support for gwf <0.4.0
- Monitors for processes and thread synchronization
- Monitors tables for sc-addresses monitors and sc-event monitors
- Distribution of sc-segments by active processes and threads
- Internal sc-storage sc-segment-built-in lists for not engaged and freed sc-elements
- Clarify events managers for sc-storage: pick out subscription and emission managers
- Clarify context manager for sc-memory
- Common secured interface for sc-memory
- GetDecompositionAgent

### Fixed

- Warnings for GNU compilers
- Warnings in sc-machine about deprecated methods
- Behavior of the agent_erase_elements on not his action classes
- Deleting existing and translated .scs sources with the same name
- Parse contours identifiers by gwf2scs-translator
- Symmetric sc-edges search
- sc-machine crashes when trying to view a picture in sc-web ([311](https://github.com/ostis-ai/sc-machine/issues/311))
- No tests for ScMemory::Initialize(sc_memory_params) with different parameters ([216](https://github.com/ostis-ai/sc-machine/issues/216))
- Too big number in websocket request crusher sc-server ([309](https://github.com/ostis-ai/sc-machine/issues/309))
- Handle crush errors ([63](https://github.com/ostis-ai/sc-machine/issues/63))
- Usage of invalid addresses after rerun failed sc-server ([79](https://github.com/ostis-ai/sc-machine/issues/79))
- Invalid addrs crushes sc-iterator methods usages ([66](https://github.com/ostis-ai/sc-machine/issues/66))
- All deadlocks and segfaults in sc-storage, sc-events and fs-memory ([111](https://github.com/ostis-ai/sc-machine/issues/111))
- Processes and thread starvation
- Active waiting during sc-events deletion
- List items semicolons for sc.s-contours
- Internal sentences after sc.s-contour assignment
- The success of finishing the action is marked first and only then that it was finished
- Finish agent wait time
- SC.g-elements images in docs

### Deprecated

- Method `getLinkContent` in sc-utils 

### Removed

- Deprecated sc-utils in 0.6.0, 0.7.0 and 0.8.0
- Deprecated make_all.sh
- Runtime asserts usage in sc-storage
- Atomic operations usage in sc-storage
- Locks usage in sc-storage

## [0.8.0-Fusion] - 24.09.2023

### Breaking changes

- Now we use C++17 instead of C++14
- We support compatibility with old memory binaries

### Added

- Add and get addresses by variable addresses in ScTemplateParams API
- Check sc-types in sc-memory API sc-elements creation methods
- Ability do not search for sc-links by substrings globally, passing config param `search_by_substring`
- Ability do not search for sc-links by strings locally, passing param in SetLinkContent `is_searchable`
- Generalize all scripts for applied projects
- Script `build_sc_machine.sh` with arguments `-f` `-t` and `-r` instead of `make_all.sh`
- View kb fragment in scn if it is keyword
- Tests for repo-path parser and sources collector in `sc-builder`
- Support for deprecated memory binaries build in 0.7.0 version
- Synchronize processes that works with file memory and sc-dictionary
- Get replacements in template result by variable addresses
- Divide strings file into more small files with fixed size by requested configs `max_strings_channels` and `max_strings_channel_size`
- Configuration of docker-compose.yml using .env file
- Script `run_tests.sh`
- Full tests for file memory API
- Intersect/unite search sc-links by strings and substrings in file memory
- Search strings and sc-link hashes by terms in file memory
- File memory on file system, save strings in file system disks
- Configure events and agents threads in sc-memory, passing config param `max_events_and_agents_threads`
- Loop-ranged API method for sc-template search ([78](https://github.com/ostis-ai/sc-machine/issues/78))
- Full tests for sc-template API
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
- Save sc-memory by period in sc-server, passing config param `save_period` in seconds
- Dump statistics by period in sc-server, passing config param `update_period` in seconds
- Agent for erasing sc-elements not belonging to the global structure `init_memory_generated_structure`
- Transfer init memory generated structure to modules initialization, passing config params `init_memory_generated_upload` and `init_memory_generated_structure`
- Possibility to skip keys defined in .ini config file in ScConfig
- Pre-commit code style checks for C++ and Python
- Add reversed edges according to standart
- Sc-links classes `![]!` in SCs
- Insert sc-keynodes and their system identifiers in global knowledge base during sc-memory initiation
- Extend create elements by SCs in `sc-server` with outputStructure field
- Write docs for system identifier set/get/find/resolve sc-memory API
- Extend set/get/find/resolve sc-memory API with outer system identifier fiver parameters
- CI for documentation build
- Add output structure builder feature to config
- Add validation of system identifier
- Append generated elements by `SCs-helper` into requested output structure
- Oriented sets representation (<a, ..., b>) in SCs
- Build docs in SCn format separately from `ostis-web-platform`

### Changed

- Replace asserts in sc-memory API by exceptions throwing
- Refactor sc-server logs
- Decrease wait time for sc-element referencing in iterators
- Call gwf-translator in sc-builder
- Move repo path parser from Python to C++ in sc-builder
- Unite ScTemplateGenResult and ScTemplateSearchResultItem into ScTemplateResultItem
- Remove list to array translations in C API for file memory
- Upload text and binary files in realtime by get API
- Remove string lengths recount in file memory
- Optimize int to string translation in file memory
- Use iterators instead of sc-template in `getNextFromSet` util
- Generalized `docker_entrypoint.sh`, this script can be used by external projects now
- Now in tests all agents works in single thread
- Replace asserts in sc-template search and gen API by exceptions throwing
- `ScsLoader::loadScsFile` return bool instead void
- Update garbage deletion agent logic, add check if element belongs to init memory structure
- Make scsi pre-processor migrations script
- Move to C++17
- Remove 1 minute assert in `ScWait`
- Add language parameter for sc2scs-json-translator to get elements identifiers
- Append to sc2scs-json elements main/system identifiers
- Replace row strings by nlohmann-json in sc2scs-json-translator

### Fixed

- Check OS type in `install_dependencies.sh`
- Check apt command for Linux OS in `install_deps_ubuntu.sh`
- Create sc-links with ScType::Link type in Debug mode
- Change memory statistics fields types from sc_uint32 to sc_uint64
- Return empty sc_addr if not found by identifier in `sc-helper`
- Build kb script doesn't ignore whitespace ([239](https://github.com/ostis-ai/sc-machine/issues/239))
- Docker entrypoint throws integer expression expected ([267](https://github.com/ostis-ai/sc-machine/issues/267))
- KB directory coping to prepare gwf and scsi sources 
- Tests for template build and gen with params
- Aliases checking in build by scs
- Not use system identifiers of vars in sc-template-build
- Sc-template search by empty template ([65](https://github.com/ostis-ai/sc-machine/issues/65))
- Segfaults in test sc-client
- Creating sc-keynodes without defining type. Now they will be created with const node type.
- Converting to string params from configs
- Fix hashes size in fs-storage ([45](https://github.com/ostis-ai/ostis-web-platform/issues/45))
- GetLinkContent skip whitespaces in strings
- No inserting system identifiers of keynodes to output structure ([223](https://github.com/ostis-ai/sc-machine/issues/223))
- `SCs-helper` doesn't return generated identifier links ([207](https://github.com/ostis-ai/sc-machine/issues/207))
- No json string message parsing in `sc-server` (WebSocket request dumps sc-machine) ([190](https://github.com/ostis-ai/sc-machine/issues/190))
- SCs-file dumps kb-builder by WebSocket ([189](https://github.com/ostis-ai/sc-machine/issues/189))
- `SCs-parser` create structure twice
- `SCs-parser` doesn't unite all generated elements into structure ([205](https://github.com/ostis-ai/sc-machine/issues/205))
- Link deletion ([193](https://github.com/ostis-ai/sc-machine/issues/193))
- Add curl to installation script
- Fix memory clear parameter reading from .ini config file
- Merging identifiers in sc-dictionary ([192](https://github.com/ostis-ai/sc-machine/issues/192))
- Dependence order in var template search triples ([186](https://github.com/ostis-ai/sc-machine/issues/186))

### Removed

- Windows files and cmake dependencies
- Prepare kb step and `prepare_kb.py`
- Rocksdb file memory
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
