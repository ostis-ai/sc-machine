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
