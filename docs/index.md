# Documentation

- **Build**
    * [Build on Linux](build/linux-build.md) - *installation manual of sc-machine on Linux-based systems*
    * [Build on OSX](build/osx-build.md) - *installation manual of sc-machine on OSX-based systems*
    * [CMake Flags](build/cmake-flags.md) - *description of CMake flags using to configure sc-machine*
    * [Configuration File](build/config.md) - *description of a configuration file of sc-machine*
- **API**
    * C++ API - *documentation of C++ API provided by sc-machine*
        * [C++ Core API](sc-memory/api/cpp/core/api.md) - *documentation of C++ core API for creating, retrieving and erasing sc-elements in sc-memory*
        * C++ Extended API: - *documentation of C++ extended API for manipulating with system identifiers of sc-elements and sc-templates*
            * [System identifier C++ API](sc-memory/api/cpp/extended/helper-api.md) - *documentation of C++ extended API for manipulating with system identifiers of sc-elements*
            * [ScTemplate C++ API](sc-memory/api/cpp/extended/template-api.md) - *documentation of C++ extended API for creating and retrieving graphs (it is very useful when you want to work big sc-constructions)*
        * [C++ Agents API](sc-memory/api/cpp/agents.md) - *documentation of C++ Agents API for creating sc-agents*
    * Python API - *documentation of Python API provided by py-sc-client*
        * [Python Core API](https://github.com/ostis-ai/py-sc-client) - *documentation of Python core API for creating, retrieving and erasing sc-elements and sc-events in sc-memory*
        * [Python Extended API](https://github.com/ostis-ai/py-sc-kpm) - *documentation of Python extended API for manipulating with large graphs*
    * TypeScript API - *documentation of TypeScript API provided by ts-sc-client*
        * [TypeScript Core API](https://github.com/ostis-ai/ts-sc-client) - *documentation of TypeScript core API for creating, retrieving and erasing sc-elements and sc-events in sc-memory*
    * [Sc-element types](scs/sc-element-types.md) - *detailed description of supported element types*
    * [SCs-code](scs/scs.md) - *documentation on SCs Language with examples*
- **Tools**
    * [Knowledge Base builder](sc-tools/sc-builder.md) - *description of options of sc-builder for knowledge base sources*
    * [Knowledge Base repo file](sc-tools/kb-repo-file.md) - *description of configuration of knowledge base sources*
    * [Websocket server](sc-tools/sc-server.md) - *description of options of sc-server for communication with sc-memory through network*
    * [Meta Programming](sc-tools/cpp-meta.md) - *documentation of meta programming language*
- **Development**
    * [Git workflow](dev/git-workflow.md) - *guide for those who wants to make contribution into sc-machine*
    * [Codestyle](dev/codestyle.md) - *guide for those who wants to write code for sc-machine*
