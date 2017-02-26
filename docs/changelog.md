## v0.3.0 rc1

* Implement SCs-text parser
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
