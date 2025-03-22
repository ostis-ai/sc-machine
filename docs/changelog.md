# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Fixed

- Add using --dev flag during launching install_deps_ubuntu.sh in quick_start for users

## [0.10.1] - 15.03.2025

### Added

- Support direct quasibinary relations in semantic neighborhood agent
- Developer tool to review PRs -- Ellipsis

### Changed

- Rename Conan remote repository url to https://conan.ostis.net/artifactory/api/conan/ostis-ai-library

### Fixed

- Translating sc-structures to json in sc2scn-json-translator
- Take into account order relations between sc-elements in quasibinary tuples in sc2scn-json-translator
- SC_LOG_INFO_COLOR uses correct enum value
- Move CONTRIBUTING.md to docs folder
- Don't install transitive dependencies of libxml2: zlib and iconv
- Fix typos in C++ Simple Guide for Implementing Agent and User permissions API
- Migration script does not affect agent logs

## [0.10.0] - 21.01.2025

### Breaking changes

- Python dependencies are set up locally in `.venv` environment. Therefore, you should make the following changes: 
    - fix `docker-compose.yml` file to run `healthcheck.py` for sc-server in `.venv` or use `scripts/healthcheck.sh` that encapsulates this logic;
    - and use `.venv` to launch your python scripts if they use sc-machine python dependencies.
- Build system of the sc-machine was upgraded:
    - The way to manage build dependencies was changed to [Conan](https://conan.io). You can install sc-machine and its dependencies with Conan. You don't have to worry about installing sc-machine dependencies on your OS anymore. See how to do it -- [Build System](build/build_system.md).
    - CMake presets should be used to build the sc-machine. They are located in `CMakePresets.json`.
    - Location of the sc-machine build was changed. The following new locations are:
        - build tree -- `build/<Debug|Release>`, 
        - binaries -- `build/<Debug|Release>/bin`, 
        - libraries -- `build/<Debug|Release>/lib`,
        - extensions -- `build/<Debug|Release>/lib/extensions`.
    - Using sc-machine as a library is much more optimized: adding it to the CMake tree is no longer necessary. You can install sc-machine packages and import sc-machine targets into your cmake using `find_package(sc-machine REQUIRED)`. See how to do it -- [Build System](build/build_system.md)
    - Each release sc-machine binaries are being compiled for supported OS and formed as an archives on Github. Minimum required version of macOS is macOS-14 (arm), of ubuntu is ubuntu-22.04. The sc-machine doesn't support ubuntu-20.04 anymore. You can use sc-machine binaries to work with sc-memory or you can use `RunMachine` method from `sc-machine-runner.so` to create your own entry point to initialize sc-memory instead of using compiled `sc-machine` binary.
    - Script for the project build (`build_sc_machine.sh`), scripts for running binaries (`run_sc_server.sh`, `build_kb.sh`) were removed from the sc-machine repository scripts. You should use sc-machine binaries directly.
    - sc-server is no longer entry point of the sc-machine, it is an extension (`sc-server-lib.so`), that is loaded dynamically when the machine is started. So, `sc-server` binary was removed, `sc-machine` binary was added instead. To specify host and port for sc-server without using config, use environment variables `SC_SERVER_HOST` and `SC_SERVER_PORT` accordingly.
- Config was changed:
    - `repo_path` option in `[sc-memory]` group was deprecated, `storage` option was added instead;
    - `extensions_path` option in `[sc-memory]` group was deprecated, `extensions` option was added instead.
- Sources of the sc-machine were separated into public and private ones. Public sources of targets are located in `include` directory, private ones - in `src` directory, tests for code - in `tests` directory. Private sources can't be included into project's code directly. Besides that there are changes of location of some API classes of the sc-machine:
    - `ScTest` class was made a part of `sc-memory` target. Use `#include <sc-memory/test/sc_test.hpp>` to include it into code.
    - To include `ScsLoader` class into code, use `#include <sc-builder/sc_loader.hpp>`.
- Configuration of sc-machine tests was changed:
    - Paths to test sources of the knowledge base, test configs, etc. are being configured at the pre-run stage, not at the stage of compiling tests.
    - Working directory for tests was changed to a directory where tests are located. To run the tests, please use `ctest` rather than the test binaries themselves.
    - Scripts for running tests (`run_tests.sh`) was removed. Go to `build/<Debug|Release>` and use `ctest` instead.
- All questions was renamed to actions.
- ScAddrHashFunc with template argument was removed. Use non-template ScAddrHashFunc without arguments. It is more safe in use.
- Constructors for ScMemoryContext with string and int parameters were removed. Use the one without parameters instead.
- HelperSearchTemplateInStruct was removed from ScMemoryContext API. Use SearchByTemplate with callbacks.
- Deprecated sc-utils in 0.9.0 were removed from sc-kpm.
- Questions were renamed to actions, answers were renamed to results.
- `m_memoryCtx` in ScAgent renamed to `m_context`.
- Сode generation is no longer used as metaprogramming. The API for agents has been completely redesigned. See the documentation section on how to implement an agent with the new API -- [C++ Agent Guide](sc-memory/api/cpp/guides/simple_guide_for_implementing_agent.md). We have
    - completely moved from code generation to template programming;
    - improved the API and aligned with our description of how it should be;
    - simplified the API. Now it will be much easier to create agents, go right now and see how to work with the new API -- [C++ Agents API](sc-memory/api/cpp/extended/agents/agents.md).
- All `ScMemoryContext` methods were redesigned to a common style. All methods with not correct names were deprecated, and new ones were added. See the table below to see which methods have been replaced.
  
  | Deprecated method                   | Substitution method                   |
  |-------------------------------------|---------------------------------------|
  | CreateNode                          | GenerateNode                          |
  | CreateLink                          | GenerateLink                          |
  | CreateEdge                          | GenerateConnector                     |
  | GetElementOutputArcsCount           | GetElementEdgesAndOutgoingArcsCount   |
  | GetElementInputArcsCount            | GetElementEdgesAndIncomingArcsCount   |
  | GetEdgeSource                       | GetArcSourceElement                   |
  | GetEdgeTarget                       | GetArcTargetElement                   |
  | GetEdgeInfo                         | GetConnectorIncidentElements          |
  | Iterator3                           | CreateIterator3                       |
  | Iterator5                           | CreateIterator5                       |
  | ForEachIter3                        | ForEach                               |
  | ForEachIter5                        | ForEach                               |
  | HelperCheckEdge                     | CheckConnector                        |
  | FindLinksByContent                  | SearchLinksByContent                  |
  | FindLinksByContentSubstring         | SearchLinksByContentSubstring         |
  | FindLinksContentsByContentSubstring | SearchLinksContentsByContentSubstring |
  | HelperSetSystemIdtf                 | SetElementSystemIdentifier            |
  | HelperGetSystemIdtf                 | GetElementSystemIdentifier            |
  | HelperResolveSystemIdtf             | ResolveElementSystemIdentifier        |
  | HelperFindBySystemIdtf              | SearchElementBySystemIdentifier       |
  | HelperGenTemplate                   | GenerateByTemplate                    |
  | HelperSearchTemplate                | SearchByTemplate                      |
  | HelperSmartSearchTemplate           | SearchByTemplateInterruptibly         |
  | HelperBuildTemplate                 | BuildTemplate                         |
  | CalculateStat                       | CalculateStatistics                   |

  | Removed method                      | Substitution method                   |
  |-------------------------------------|---------------------------------------|
  | BeingEventsPending                  | BeginEventsPending                    |

- All C++ sc-types were also redesigned to a common style. They were deprecated, new ones were added. See changes in the table below.
  
  | Deprecated                     | Substitution               |
  |--------------------------------|----------------------------|
  | ScType::EdgeUCommon            | ScType::CommonEdge         |
  | ScType::EdgeDCommon            | ScType::CommonArc          |
  | ScType::EdgeUCommonConst       | ScType::ConstCommonEdge    |
  | ScType::EdgeDCommonConst       | ScType::ConstCommonArc     |
  | ScType::EdgeAccess             | ScType::MembershipArc      |
  | ScType::EdgeAccessConstPosPerm | ScType::ConstPermPosArc    |
  | ScType::EdgeAccessConstNegPerm | ScType::ConstPermNegArc    |
  | ScType::EdgeAccessConstFuzPerm | ScType::ConstFuzArc        |
  | ScType::EdgeAccessConstPosTemp | ScType::ConstTempPosArc    |
  | ScType::EdgeAccessConstNegTemp | ScType::ConstTempNegArc    |
  | ScType::EdgeAccessConstFuzTemp | ScType::ConstFuzArc        |
  | ScType::EdgeUCommonVar         | ScType::VarCommonEdge      |
  | ScType::EdgeDCommonVar         | ScType::VarCommonArc       |
  | ScType::EdgeAccessVarPosPerm   | ScType::VarPermPosArc      |
  | ScType::EdgeAccessVarNegPerm   | ScType::VarPermNegArc      |
  | ScType::EdgeAccessVarFuzPerm   | ScType::VarFuzArc          |
  | ScType::EdgeAccessVarPosTemp   | ScType::VarTempPosArc      |
  | ScType::EdgeAccessVarNegTemp   | ScType::VarTempNegArc      |
  | ScType::EdgeAccessVarFuzTemp   | ScType::VarFuzArc          |
  | ScType::NodeConst              | ScType::ConstNode          |
  | ScType::NodeVar                | ScType::VarNode            |
  | ScType::Link                   | ScType::NodeLink           |
  | ScType::LinkClass              | ScType::NodeLinkClass      |
  | ScType::NodeStruct             | ScType::NodeStructure      |
  | ScType::LinkConst              | ScType::ConstNodeLink      |
  | ScType::LinkConstClass         | ScType::ConstNodeLinkClass |
  | ScType::NodeConstTuple         | ScType::ConstNodeTuple     |
  | ScType::NodeConstStruct        | ScType::ConstNodeStructure |
  | ScType::NodeConstRole          | ScType::ConstNodeRole      |
  | ScType::NodeConstNoRole        | ScType::ConstNodeNonRole   |
  | ScType::NodeConstClass         | ScType::ConstNodeClass     |
  | ScType::NodeConstMaterial      | ScType::ConstNodeMaterial  |
  | ScType::LinkVar                | ScType::VarNodeLink        |
  | ScType::LinkVarClass           | ScType::VarNodeLinkClass   |
  | ScType::NodeVarStruct          | ScType::VarNodeStructure   |
  | ScType::NodeVarTuple           | ScType::VarNodeTuple       |
  | ScType::NodeVarRole            | ScType::VarNodeRole        |
  | ScType::NodeVarNoRole          | ScType::VarNodeNonRole     |
  | ScType::NodeVarClass           | ScType::VarNodeClass       |
  | ScType::NodeVarMaterial        | ScType::VarNodeMaterial    |

  From now on, all sc-links are sc-nodes. Types of actual and inactual temporal membership sc-arc were added. 
  Also, all possible combinations of subtypes in sc-types have been added to the API of ScMemoryContext and to the SCs-code.

- Incorrect sc.s-connectors were replaced by correct ones.

  | Deprecated | Substitution |
  |------------|--------------|
  | ```>```    | ```?=>```    |
  | ```<```    | ```<=?```    |
  | ```<>```   | ```?<=>```   |
  | ```_<=```  | ```<=_```    |
  | ```_<-```  | ```<-_```    |
  | ```_<|-``` | ```<|-_```   |
  | ```_<~```  | ```<~_```    |
  | ```_<|~``` | ```<|~_```   |

- It is no longer possible to specify permanency for fuzzy arcs, because it may lead to misunderstanding of the non-factor denoted by this sc-arc. So, designations ```-/>```, ```</-```, ```_-/>```, ```_</-```, ```</-_```, ```~/>```, ```</~```, ```_~/>```, ```_</~```, ```</~_``` were removed. Use ```/>```, ```</```, ```_/>```, ```</_``` instead.

- Incorrect system identifiers of sc.s-keynodes were also replaced by correct ones.

  | Deprecated              | Substitution              |
  |-------------------------|---------------------------|
  | sc_edge                 | sc_common_edge            |
  | sc_edge_ucommon         | sc_common_edge            |
  | sc_arc_common           | sc_common_arc             |
  | sc_edge_dcommon         | sc_common_arc             |
  | sc_arc_access           | sc_membership_arc         |
  | sc_edge_access          | sc_membership_arc         |
  | sc_arc_main             | sc_main_arc               |
  | sc_edge_main            | sc_main_arc               |
  | sc_node_struct          | sc_node_structure         |
  | sc_node_norole_relation | sc_node_non_role_relation |

- Type `ScType::NodeAbstract` and sc.s-keynode `sc_node_abstract` were removed.

See documentation, to learn more about using new API.

### Added

- Environment variables `SC_SERVER_HOST` and `SC_SERVER_PORT` for sc-server
- `SearchLinksByContentSubstring` method with `ScLinkFilter` parameter
- `ScLinkFilter` class to specify search criteria for sc-links
- Intro for documentation
- Quick start section for developers in docs
- Quick start section for users in docs
- Documentation for user permissions
- Allow multiple extension directories for sc-memory
- CD for publishing sc-machine binaries as archive on Github 
- CI for checking sc-machine tests build with Conan dependencies
- Install target to prepare consuming sc-machine targets
- Conan support for managing sc-machine dependencies
- Handling required options for sc-machine and sc-builder
- Display versions for sc-machine and sc-builder
- Doc strings for sc-config classes
- Implement gwf2scs-translator in cpp
- `GetSCsElementKeynode` method for sc-types to get their sc.s-keynode system identifiers
- `GetDirectSCsConnector` and `GetReverseSCsConnector` for sc-types to get their designations in SCs-code
- std::string operator for ScType
- All possible semantic combinations of subtypes in sc-types into the SCs-code
- All possible combinations of subtypes in sc-types into ScMemoryContext API
- Type `ScType::NodeSuperclass` and sc.s-keynode `sc_node_superclass`
- Types: `ScType::Connector`, `ScType::Arc`
- Types of actual and inactual temporal sc-arcs into ScMemoryContext API and the SCs-code
- Methods in ScMemoryContext: GenerateNode, GenerateLink, GenerateConnector, GetElementEdgesAndOutgoingArcsCount, GetElementEdgesAndIncomingArcsCount, GetArcSourceElement, GetArcTargetElement, GetConnectorIncidentElements, CreateIterator3, CreateIterator5, ForEach, CheckConnector, SearchLinksByContent, SearchLinksByContentSubstring, SearchLinksContentsByContentSubstring, SetElementSystemIdentifier, GetElementSystemIdentifier, ResolveElementSystemIdentifier, SearchElementBySystemIdentifier, GenerateByTemplate, SearchByTemplate, SearchByTemplateInterruptibly, BuildTemplate, CalculateStatistics, BeginEventsPending
- Simple guide for implementing agent in C++
- Documentation for agents, keynodes, modules, events, subscriptions, waiters, actions and agent context
- Full tests for C++ Agents API
- Opportunity to implement agents with static, dynamic and semi-dynamic specifications
- Implement `ScAgentContext` class to work with sc-event, subscriptions and waiters
- Implement `ScAction` class to work with sc-actions in sc-memory
- Implement `ScAgentBuilder` to handle dynamic specifications for agents
- Implement `ScModule` to register agents
- New implementation of `ScKeynodes` class to enumerate sc-keynodes
- Implement `ScTemplateKeynode` class to represent static object of sc-template
- Implement `ScKeynode` class to represent static object of key sc-element used in agents
- New implementation of `ScAgent` class for any types of agents
- Implement `ScActionInitiatedAgent` class to implement agents interpreting initiated actions
- Implement sc-event classes hierarchy
- Implement new C++ Agents API based on template programming
- Add unordered_map type with `ScAddr` as key
- Implement protected method in ScMemoryContext to translate object of `ScTemplate` to sc-template in sc-memory
- Doc strings for public template classes
- Usings `ScAddrStack`, `ScAddrQueue`, `ScAddrSet` and `ScAddrHashSet`
- Doc strings for iterator classes
- Clean monitor tables by size threshold
- Compile option to optimize checking local user permissions
- Check incidence between sc-connectors and sc-elements substituted into sc-template from sc-template params
- Generate and search by sc-templates with constant sc-connectors
- Build sc-templates with constant sc-connectors
- Opportunity to substitute sc-connectors as params in sc-templates
- Script `healthcheck.sh` for `healthcheck.py`
- Check build and tests on runner-image `ubuntu-24.04`
- Support for ubuntu-24.04 and macOS 14.4.1
- Block events mode for sc-memory context; methods `BeginEventsBlocking` and `EndEventsBlocking` in `ScMemoryContext`
- Opportunity to set permissions for set of users
- Guests identification
- Create guest users during creating sc-memory context
- Get user address for sc-server session and sc-memory context
- Docker entrypoint run to launch binary `sc-machine` in docker
- Binary `sc-machine` instead of binary `sc-server` that loads extension sc-server
- Collect user permissions during sc-memory initialize
- Throw exceptions in get values methods of iterators if sc-element is not accessed
- Callback to get and collect sc-links by its contents from fs-memory
- Local user permissions for actions within sc-structures
- List in sc-element for input arcs from sc-structures
- Provide sc-arc types for sc-event callbacks
- User permissions for handling permissions for actions in sc-memory
- Global user permissions for actions in sc-memory
- ScType::ConstNodeLinkClass and ScType::VarNodeLinkClass
- User authentication checks
- Denote sc-machine with sc-element `myself`
- Provide users for sc-memory, sc-events and sc-agents

### Changed

- Improve docs usability: add search, edit links, coping code blocks, logo, text justifying, move sections to header and other 
- SCsParser no longer generates sc-arcs from types of sc-elements to sc-elements
- Allow multiple instances of `ScLogger` class
- Rename `ScLog` to `ScLogger`
- Description of project in Readme
- Working directory for each test has been changed to the test's source dir
- `gtest` and `benchmark` are installed via Conan or OS package managers instead of using them as git submodules
- Location of the sc-machine build tree, binaries, libraries and extensions
- Bumped minimum required cmake version to 3.24.0
- Help messages for sc-machine and sc-builder
- Clarify sc-machine and sc-builder options
- Specify dependencies near targets, not in overall file
- Make glib dependencies private
- Replace public sc-base defines with methods
- Separate public and private sources
- Rename action answer to action result
- Rename `ScWait` to `ScWaiter`
- Rename `ScEvent` to `ScEventSubscription`
- Make template build methods to throw exceptions instead of return errors
- Rename questions to actions
- Use queues in monitors statically
- Implement array-based sc-queue
- Clarify error message for building sc-template, generating and searching by sc-template: provide sc-template item features in error message
- Reformat code style according to clang-format-18
- Use runner-image `macos-14` instead of runner-image `macos-latest` in CI
- Don't check extensions when loading dynamic modules (`g_open_module` does it automatically)
- Use venv for python dependencies in scripts and docker
- Don't emit events after kb build
- Allows assigns sc-link to its system identifier

### Fixed

- Validate parameters in handle link content sc-json-command in sc-server
- Iterating sc-connectors with sc-edge loop
- Checking of all syntactic and semantic subtypes for types in `ScMemoryContext::SetElementSubtype` and `ScType::CanExtendTo` methods.
- Now sc-link is sc-node
- sc-arcs and sc-elements are removed after agents have worked with them
- fs-memory uses monitor to resolve string offset
- fs-memory searches for new strings in all channels instead of only in last channel
- Separate implementation from declarations in templates
- Separate specializations from declarations in iterators
- Fix duplication of iterator specializations
- Make sc_addr_hash equal to sc_uint32
- Make core literal types platform-independent
- Erasing sc-connectors during iterating
- Clear result of generating by sc-template if error is occurred
- Check that template params sc-type can be extended to template item sc-type
- No discard return types for sc-template API
- C++ warnings for ubuntu-24.04 and macOS-14.4.1 when building project
- Use `snprintf(3)` instead of `sprintf(2)`
- Private and public dependencies between cmake targets
- Use reenterable strtok
- Resolve sc.g-links with the same system identifier

### Deprecated

- Config option `repo_path` in `[sc-memory]`, add option `storage` instead
- Config option `extensions_path` in `[sc-memory]`, add option `extensions` instead
- Incorrect designations of sc.s-connectors: `>`, `<`, `<>`, `_<=`, `_<-`, `_<|-`, `_<~`, `_<|~`
- Incorrect sc.s-keynodes: `sc_edge`, `sc_edge_ucommon`, `sc_arc_common`, `sc_edge_dcommon`, `sc_arc_access`, `sc_edge_access`, `sc_arc_main`, `sc_edge_main`, `sc_node_struct`, `sc_node_norole_relation`
- C++ sc-types: `ScType::EdgeUCommon`, `ScType::EdgeDCommon`, `ScType::EdgeUCommonConst`, `ScType::EdgeDCommonConst`, `ScType::EdgeAccess`, `ScType::EdgeAccessConstPosPerm`, `ScType::EdgeAccessConstNegPerm`, `ScType::EdgeAccessConstFuzPerm`, `ScType::EdgeAccessConstPosTemp`, `ScType::EdgeAccessConstNegTemp`, `ScType::EdgeAccessConstFuzTemp`, `ScType::EdgeUCommonVar`, `ScType::EdgeDCommonVar`, `ScType::EdgeAccessVarPosPerm`, `ScType::EdgeAccessVarNegPerm`, `ScType::EdgeAccessVarFuzPerm`, `ScType::EdgeAccessVarPosTemp`, `ScType::EdgeAccessVarNegTemp`, `ScType::EdgeAccessVarFuzTemp`, `ScType::NodeConst`, `ScType::NodeVar`, `ScType::Link`, `ScType::LinkClass`, `ScType::NodeStruct`, `ScType::LinkConst`, `ScType::LinkConstClass`, `ScType::NodeConstTuple`, `ScType::NodeConstStruct`, `ScType::NodeConstRole`, `ScType::NodeConstNoRole`, `ScType::NodeConstClass`, `ScType::NodeConstMaterial`, `ScType::LinkVar`, `ScType::LinkVarClass`, `ScType::NodeVarStruct`, `ScType::NodeVarTuple`, `ScType::NodeVarRole`, `ScType::NodeVarNoRole`, `ScType::NodeVarClass`, `ScType::NodeVarMaterial`
- Methods of `ScMemoryContext`: CreateNode, CreateLink, CreateEdge,GetElementOutputArcsCount, GetElementInputArcsCount, GetEdgeSource, GetEdgeTarget, GetEdgeInfo, Iterator3, Iterator5, ForEachIter3, ForEachIter5, HelperCheckEdge, FindLinksByContent, FindLinksByContentSubstring, FindLinksContentsByContentSubstring, HelperSetSystemIdtf, HelperGetSystemIdtf, HelperResolveSystemIdtf, HelperFindBySystemIdtf, HelperGenTemplate, HelperSearchTemplate, HelperSmartSearchTemplate, HelperBuildTemplate, CalculateStat

### Removed

- Support for ubuntu-20.04
- Config options `update_period` and `save_period` in `[sc-memory]` deprecated in 0.9.0
- Config option `sync_actions` in `[sc-server]` deprecated in 0.9.0
- Scripts: `build_sc_machine.sh`, `build_kb.sh`, `run_sc_server.sh`, `set_vars.sh` and `run_tests.sh`
- `sc-server` binary and docker entrypoint command `serve`
- gwf2scs-translator in python
- Designations of fuzzy sc.s-arcs: ```-/>```, ```</-```, ```_-/>```, ```_</-```, ```</-_```, ```~/>```, ```</~```, ```_~/>```, ```_</~```, ```</~_```
- Type `ScType::NodeAbstract` and sc.s-keynode `sc_node_abstract`
- Methods of `BeingEventsPending`: BeingEventsPending
- Codegen and C++ Agents API based on code generation
- Deprecated sc-utils in 0.9.0
- Constructors for ScMemoryContext with string and int parameters
- HelperSearchTemplateInStruct from ScMemoryContext
- Template argument for `ScAddrHashFunc`
- Throw `utils::ExceptionInvalidParams` if template params have sc-connectors substitution for generating by sc-template
- GetDecompositionAgent
- Sc-links lists translation from C API to C++ API

## [0.9.0-Unlock] - 22.01.2024

### Breaking changes

- All methods in C++ API for managing sc-memory throws exceptions if passed params are not valid or memory state is not valid
- All methods in C API for managing sc-memory returns error codes if passed params are not valid or memory state is not valid
- Removed deprecated sc-utils in 0.6.0, 0.7.0 and 0.8.0
- Removed deprecated make_all.sh

### Added

- Config option `limit_max_threads_by_max_physical_cores` in `[sc-memory]` to limit max threads by max physical threads
- Config options `dump_memory` and `dump_memory_statistics` in `[sc-memory]` for enabling sc-memory dumps
- Full md docs for sc-memory API on C++ with examples
- ScTemplate replacement presence check using varAddr
- CreateIterator5 AAAAF
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

### Changed

- Optimize and move sc-memory dumps from sc-server to sc-memory
- Refactor all code by .clang-format 18.0.0
- Replace hyphens by underscores in files
- Replace underscores by hyphens in folders
- Update codestyle guide
- Update contributing guide
- Update .clang-format to 18.0.0

### Fixed

- Separate setups for sc-server and sc-builder from main.cpp and test
- Script scg.min.js styles handling for sc.g-element types of KBE >0.4.0
- Sc.g-elements images in docs
- Support recursive curl braces in SCs-code level 2
- Support semantic sc-types for sc-node in SCs-code level 1
- Support sc-links for SCs-code level 1
- HelperBuildTemplate with params that have varAddr replacements
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
- Success of finishing the action is marked first and only then that it was finished
- Finish agent wait time

### Deprecated

- Config option `sync_actions` in `[sc-server]`, add option `parallel_actions` instead of
- Config option `update_period` in `[sc-memory]`, add option `dump_memory_statistics_period` instead of
- Config option `save_period` in `[sc-memory]`, add option `dump_memory_period` instead of
- Method `getLinkContent` in sc-utils 

### Removed

- `sc::ResolveRelationTuple` and `sc::SetRelationValue`
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
- sc-machine doesn't support ubuntu versions lower than 20.04.

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
