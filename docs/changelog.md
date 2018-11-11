## v0.6.0

 - **Build system**:
    - Add support of OSX platform
    - Fix compilation with `clang`

 - **Tools**:
    - Support custom extensions loading in `sc-builder`
    - Add `sc-server` application to run claen `sc-memory` with extensions

 - **Core**:
    - Support list of enabled extensions
    - Support all possible combinations of `sc_iterator3`

 - **C++**:
    - Implement optional search triples support
    - Fix randomical crash of unittest
    - Improve test system

 - **Python**:
    - Refactoring test system
    - Add support of template building from `SCs-code` string
    - Add `Aliases` method to `ScTemplateSearchResult` and `ScTemplateGenResult`

 - **KPM**:
    - **web**:
        - Implement `http` module to access memory via Web interface
        - Implement web browser interface to access `sc-memory`

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
