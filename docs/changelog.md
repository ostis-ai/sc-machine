# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]
- Add `;` after `SC_LOG_INFO`, `SC_LOG_DEBUG`, `SC_LOG_WARNING` and `SC_LOG_ERROR` calls in projects that use sc-machine
- Add `;` after `SC_ASSERT` calls in projects that use sc-machine

### Added
- Add ci for `rocksdb` and `sc-dictionary`
- Implement `sc-dictionary`. Add opportunity to switch `rocksdb` and `sc-dictionary`
- Wrap and separate allocating, assertion, notification and atomic lock free procedures
- Add tests module for sc-agents-utils
- Add opportunity to configure sc-machine version from CMakeLists.txt
- Add scripts to build kb, run sctp-server and run sc-server
- New flag options for build_kb
- Add default python modules paths to python initialization
- United config file
- Automatic usage of ccache to speed up builds

### Changed
- Simplify main CMakeLists.txt
- Add tools macros for main CMakeLists.txt
- Separate dependencies specifying from main CMakeLists.txt
- Correct typos
- Fix warnings in sc-memory modules
- Fix scs-grammar. Add opportunity to specificate a structure into this structure itself

### Removed
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

* Add `ScSet` and `ScRelationSet` in python common library
* Add `ScPythonService` into C++ (allow to implement threaded python services)
* Add `GetType` and `AsBinary` functions into Python for a link content
* Add colors to python output
* Add signal handler to handle `Ctrl + C`
* Fix bug with python modules search path initialization
* Fix bug with python threading
* Fix bug with builder run
* Fix bug with `ScLock`
* Fix bug with Python interpreter shutdown
* Fix bug with `int8_t` type support in python bindings
* Fix bug with template generation, when pass parameter to triple with type that has no constancy flag
* Fix errors catch during collect python modules
* Refactoring of C++ Python bridge

## v0.4.0

* Add events support in Python scripts
* Improve Python scripts run
* Add common libraries for a Python
* Fix memory leaks in core
* Improve test logging
* Improve work with memory buffers
* Cleanup API
* Add support of python 3 code
* Add `ScLockScope` class
* Implement multithreaded python script run
* Pass `cpp_bridge` into python script
* Extract `ScWaitEvent` as a separated class from `ScWait`
* Some fixes in template search
* Make log thread safe
* Implement `ScLink` wrapper class
* Add `ScBase64` encode/decode functions
* Add common templates generation (see `sc_common_templ.hpp`)

## v0.3.1

* Fix templates search error (problems was with triples search order)
* Implement logging for load extension module problems
* Remove `nl`, `iot` modules from kpm, because they are project specific (moved to a separate repo)
* Add `ForEach` function into template search result
* Add `ForEachIter` functions into `ScMemoryContext`
* Add `ScSet` class
* Add support of http requests

## v0.3.0

* Implement search for elements, that has no constancy flag in a type
* Fix template search error (#251)
* Fix problem with `SC_ASSERT` compilation in release mode (#249)
* Implement SCs-text parser (levels 1 - 4)
* Agent `AApiAiParseUserTextAgent` can run commands
* Implement common function to work with commands in knowledge base
* Add support of keynode types in codegenerator. Now you can specify type of keynode that would be used to create it: `ForceCreate(ScType::Node...)`
* Rename module funcitons from `initialize`, `shutdown`, `load_priority` to `sc_module_initialize`, `sc_module_shutdown`, `sc_module_load_priority`
* Update code to codestyle. Most functions was renamed from `functionName` to `FunctionName`. Iterator `value` function replaced with `Get`.
* Implement support of result codes for action agents
* Improve unit test system
* Add [MkDocs](http://www.mkdocs.org/) documentation generator support
* Add `clang-format` config
* Add codestyle document
* Add documentation for `C++` API
* Implement multithreaded emit of sc-events
* Implement ref counter for sc-elements
* Cover whole functionality of `C++` API by unit tests
* Implement `C++` API

[unreleased]: https://github.com/ostis-ai/sc-machine/compare/0.6.1...HEAD
[0.6.1]: https://github.com/ostis-ai/sc-machine/compare/0.6.0...0.6.1
