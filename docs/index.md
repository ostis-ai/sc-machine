# Documentation

- **Build**
    * [Build on Linux](build/linux_build.md) - *installation manual of sc-machine on Linux-based systems*
    * [Build on OSX](build/osx_build.md) - *installation manual of sc-machine on OSX-based systems*
    * [CMake Flags](build/cmake_flags.md) - *description of CMake flags using to configure sc-machine*
    * [Configuration File](build/config.md) - *description of a configuration file of sc-machine*
- **API**
    * C++ API - *documentation of C++ API provided by sc-machine*
        * [C++ Core API](sc-memory/api/cpp/core/api.md) - *documentation of C++ core API for creating, retrieving and erasing sc-elements in sc-memory*
        * C++ Extended API: - *documentation of C++ extended API for manipulating with system identifiers of sc-elements and sc-templates*
            * [C++ System identifier API](sc-memory/api/cpp/extended/helper_api.md) - *documentation of C++ extended API for manipulating with system identifiers of sc-elements*
            * [C++ ScTemplate API](sc-memory/api/cpp/extended/template_api.md) - *documentation of C++ extended API for creating and retrieving graphs (it is very useful when you want to work big sc-constructions)* 
            * C++ Agents API: - *documentation of C++ Agents API for creating sc-agents*
                * [C++ Events](sc-memory/api/cpp/extended/agents/events.md) 
                * [C++ Event subscriptions](sc-memory/api/cpp/extended/agents/event_subscriptions.md) 
                * [C++ Waits](sc-memory/api/cpp/extended/agents/waits.md) 
                * [C++ Agents](sc-memory/api/cpp/extended/agents/agents.md)
                * [C++ Keynodes]
                * [C++ Modules]
                * [C++ Agent utils]
        * C++ Guides:
            * [C++ Implementing agent guide](sc-memory/api/cpp/guides/implementing_agent_guide.md)
    * Python API - *documentation of Python API provided by py-sc-client*
        * [Python Core API](https://github.com/ostis-ai/py-sc-client) - *documentation of Python core API for creating, retrieving and erasing sc-elements and sc-events in sc-memory*
        * [Python Extended API](https://github.com/ostis-ai/py-sc-kpm) - *documentation of Python extended API for manipulating with large graphs*
    * TypeScript API - *documentation of TypeScript API provided by ts-sc-client*
        * [TypeScript Core API](https://github.com/ostis-ai/ts-sc-client) - *documentation of TypeScript core API for creating, retrieving and erasing sc-elements and sc-events in sc-memory*
    * [Sc-element types](scs/sc_element_types.md) - *detailed description of supported element types*
    * [SCs-code](scs/scs.md) - *documentation on SCs Language with examples*
- **Utils**
    * [Agents](sc-kpm/kpm.md) - *detailed description of common sc-agents*
- **Tools**
    * [Knowledge Base builder](sc-tools/sc_builder.md) - *description of options of sc-builder for knowledge base sources*
    * [Knowledge Base repo file](sc-tools/kb_repo_file.md) - *description of configuration of knowledge base sources*
    * [Websocket Server](sc-tools/sc_server.md) - *description of options of sc-server for communication with sc-memory through network*
    * [Sc-machine Runner](sc-tools/sc_machine.md) - *description of options of sc-machine runner*
- **Development**
    * [Contributing Guide](https://github.com/ostis-ai/sc-machine/blob/main/CONTRIBUTING.md) - *guide for those who wants to make contribution into sc-machine*
    * [Codestyle Guide](dev/codestyle.md) - *guide for those who wants to write code for sc-machine*
    * [Dev Container](dev/devcontainer.md) - *guide for those who wants to develop sc-machine via docker*
