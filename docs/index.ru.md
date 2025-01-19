# Documentation

Welcome to the [sc-machine](https://github.com/ostis-ai/sc-machine) documentation! This comprehensive guide is designed to help you navigate the features, functionalities, and applications of sc-machine, a C++ software package that emulates semantic computer behavior through the storage and processing of knowledge in a semantic network.

## What is sc-machine?  

At its core, sc-machine operates as a **graph database management system**, enabling users to efficiently store, retrieve, and manipulate *knowledge graphs* in common shared memory known as *sc-memory*. By leveraging an agent-based approach, it processes these graphs to facilitate complex tasks and workflows. The underlying technology is grounded in the [**OSTIS Technology**](https://github.com/ostis-ai), which allows for the representation of both declarative and procedural knowledge using a unified language known as *SC-code (Semantic Computer code)*.

Table of contents:

- [Quick Start](quick_start.md) - *get up and running with sc-machine quickly*
- [Docker](docker.md) - *get up and running with sc-machine in docker*
- **User Guides** - *detailed instructions on implementing agents*
    * C++ Guides - *guidelines for implementing agents on C++*
        * [C++ Simple Guide for Implementing Agent](sc-memory/api/cpp/guides/simple_guide_for_implementing_agent.md)
    * [Migrate to New Agent API](sc-tools/migrate_to_new_agent_api.md) - *description of script that can be used to migrate to new C++ Agents API*
- **API** - *comprehensive documentation for C++, Python, and TypeScript APIs*
    * C++ API - *detailed guides on using the core and extended APIs for interacting with sc-memory*
        * [C++ Core API](sc-memory/api/cpp/core/api.md) - *documentation of C++ core API for creating, retrieving and erasing sc-elements in sc-memory*
        * C++ Extended API - *documentation of C++ extended API for manipulating with system identifiers of sc-elements and sc-templates*
            * [C++ System Identifier API](sc-memory/api/cpp/extended/helper_api.md) - *documentation of C++ extended API for manipulating with system identifiers of sc-elements*
            * [C++ ScTemplate API](sc-memory/api/cpp/extended/template_api.md) - *documentation of C++ extended API for creating and retrieving graphs (it is very useful when you want to work big sc-constructions)*
            * [C++ User Permissions API](sc-memory/api/cpp/extended/permissions_api.md) - *documentation of handling users and their permissions in knowledge base*
            * C++ Agents API - *documentation of C++ Agents API for creating agents*
                * [C++ Events API](sc-memory/api/cpp/extended/agents/events.md) - *enables developers to create and manage events within the sc-machine, allowing agents to respond dynamically to various triggers*
                * [C++ Event Subscriptions API](sc-memory/api/cpp/extended/agents/event_subscriptions.md) - *details how agents can subscribe to specific events and receive notifications when those events occur*
                * [C++ Waiters API](sc-memory/api/cpp/extended/agents/waiters.md) - *allows developers to implement wait conditions that pause agent execution until specified conditions are met or events are triggered*
                * [C++ Keynodes API](sc-memory/api/cpp/extended/agents/keynodes.md) - *provides functionality for creating, retrieving, and manipulating keynodes, which represent significant concepts within the knowledge graph*
                * [C++ Actions API](sc-memory/api/cpp/extended/agents/actions.md) - *outlines how to define and initiate actions that agents can perform, detailing their arguments and results*
                * [C++ Agents API](sc-memory/api/cpp/extended/agents/agents.md) - *focuses on agent creation and management*
                * [C++ Agent Context API](sc-memory/api/cpp/extended/agents/agent_context.md) - *explains how to manage the context in which agents operate*
                * [C++ Modules API](sc-memory/api/cpp/extended/agents/modules.md) - *describes how to register agents in an application*
    * Python API - *documentation on using the Python client for interacting with sc-memory*
        * [Python Core API](https://github.com/ostis-ai/py-sc-client) - *documentation of Python core API for creating, retrieving and erasing sc-elements and sc-events in sc-memory*
        * [Python Extended API](https://github.com/ostis-ai/py-sc-kpm) - *documentation of Python extended API for manipulating with large graphs*
    * TypeScript API - *documentation on using the TypeScript client for interacting with sc-memory*
        * [TypeScript Core API](https://github.com/ostis-ai/ts-sc-client) - *documentation of TypeScript core API for creating, retrieving and erasing sc-elements and sc-events in sc-memory*
    * [sc-element Types](scs/sc_element_types.md) - *detailed description of supported element types*
    * [SCs-code](scs/scs.md) - *documentation on SCs Language with examples*
- **Tools** - *information on various tools that enhance your experience with sc-machine*
    * [Knowledge Base Builder](sc-tools/sc_builder.md) - *description of options of sc-builder for knowledge base sources*
    * [Knowledge Base Repo File](sc-tools/kb_repo_file.md) - *description of configuration of knowledge base sources*
    * [sc-machine Runner](sc-tools/sc_machine.md) - *description of options of sc-machine runner*
- **Build Instructions** - *guidelines for building the project, configuring settings*
    * [Quick Start for Developers](build/quick_start.md) - *get up and start developing sc-machine quickly*
    * [Build System](build/build_system.md) - *how to build the project and use it as a library*
    * [CMake Flags](build/cmake_flags.md) - *description of CMake flags used to configure sc-machine*
    * [Configuration File](build/config.md) - *description of a configuration file of sc-machine*
- **Development** - *guidelines for contributing to development*
    * [Contributing Guide](https://github.com/ostis-ai/sc-machine/blob/main/CONTRIBUTING.md) - *guide for those who wants to make contribution into sc-machine*
    * [Code Style Guide](dev/codestyle.md) - *guide for those who wants to write code for sc-machine*
    * [Dev Container](dev/devcontainer.md) - *guide for those who wants to develop sc-machine via docker*
- [License](https://github.com/ostis-ai/sc-machine/blob/main/COPYING.MIT)
- [Changelog](changelog.md)
