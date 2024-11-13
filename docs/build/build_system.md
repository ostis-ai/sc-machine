# Build system

## Implementation details

The project uses CMake as the build system generator. CMake allows for cross-platform builds and provides the necessary flexibility for both project development and its use in external projects.

[Conan](https://conan.io) is integrated as an optional dependency manager via dependency resolver mechanism introduced in CMake 3.24 (see `cmake/conan_provider.cmake`). Conan helps us manage external libraries and allows us to create reproducible developer environment with ease. However, the build system is designed to work without Conan if needed.

## Compilation steps

Typical configurations of the project are provided in the `CMakePresets.json` file – this means that if you use a modern IDE, **no additional configuration is needed**. Just pick one of the configurations provided by this file and use controls in your IDE to recompile and run tests (should work with VSCode + CMake Tools extension, JetBrains CLion and probably many others).

If you wish to use CLI instead, you can list all the available configurations in the `CMakePresets.json` file and pick one of the presets:

```sh
cmake --list-presets
```

!!! warning
    By default, the standard repositories for Ubuntu 22.04 provide CMake version 3.22, which does not support certain features required for working with CMakePresets.json. To avoid issues during the build process, you need to manually install CMake version 3.24 or higher.

For example, this is how to build sc-machine in debug mode using dependencies from Conan:

```sh
# use pipx to install Conan if not already installed
# Install pipx first using this guide: https://pipx.pypa.io/stable/installation/ 
pipx install conan
# Use preset with Conan-provided dependencies and debug build type
cmake --preset debug-conan
# Use debug build preset since we've used a debug configure preset
cmake --build --preset debug
```

This is how you would build sc-machine in release mode using system-provided dependencies:

```sh
# release build type with tests (no conan postfix = system-provided deps)
cmake --preset release-with-tests
cmake --build --preset release
```

The build artifacts will be located in the `build/<Release|Debug>/bin` folder.

Default installation folder location (used for package consumption) is `build/<Release|Debug>/install`, installation process can be fired by appending `--target install` to the `cmake --build --preset <preset>` command.

## Consuming the package

<details markdown>

<summary>find_package() using CPack artifacts on GitHub Actions</summary>

The package artifacts are available [here](https://github.com/ostis-ai/sc-machine/releases). To use these in your CMake project, download and extract the release for your platform to any convenient location. Then make it available to CMake by appending folder path to `CMAKE_PREFIX_PATH`:

```cmake
# import sc-machine from the Releases archive
# you can override this variable via -D<proj_name>_PATH_SC_MACHINE_PATH or CMakeUserPreset.json / CMakePreset.json files
set(<proj_name>_SC_MACHINE_PATH "/location/to/sc-machine-<version>-<platform>" CACHE PATH "sc-machine installation path")

# can be overriden using env variables as well
if(DEFINED ENV{<proj_name>_SC_MACHINE_PATH})
  set(<proj_name>_SC_MACHINE_PATH "$ENV{<proj_name>_SC_MACHINE_PATH}")
endif()

list(APPEND CMAKE_PREFIX_PATH ${<proj_name>_SC_MACHINE_PATH})
```
</details>

<details markdown>

<summary>find_package() using Conan</summary>

- You want to use a released version of sc-machine
  
  Simply add sc-machine as a dependency to your project and use Conan to install it for you.

  conanfile.txt:

  ```ini
  [requires]
  sc-machine/0.10.0

  [generators]
  CMakeDeps
  CMakeToolchain

  [layout]
  cmake_layout
  ```

  ```sh
  conan install .
  cmake --preset conan-release
  cmake --build --preset conan-release
  ```

- You want to use a package version that is not published in a Conan repo:
  
  Export the version you'd like to use to local conan cache:
  ```sh
  git clone https://github.com/ostis-ai/sc-machine
  git checkout <your-commit>
  cmake --preset release-conan
  cmake --build --preset release --target install
  conan export-pkg .
  # you should see the package version and revision exported to local conan cache
  ```

  Then in your project edit conanfile.txt and add the following:

  ```ini
  [requires]
  # git commit is also the revision
  sc-machine/0.10.0#0c1889bc6a34450c7728b4b12af9083910cebb36

  [generators]
  CMakeDeps
  CMakeToolchain

  [layout]
  cmake_layout
  ```

  Finally, build your project

  ```sh
  conan install .
  cmake --preset conan-release
  cmake --build --preset conan-release
  ```

</details>

After making sc-machine available to find_package() in any of the above ways, use this to import our targets:

```cmake
find_package(sc-machine REQUIRED)
```

After adding this code to your build system, using sc-machine libraries and executables will be available to you. Linking to a library will automatically make headers available to you as well.

Let's try to use it!

```cmake
add_executable(wave-find-path main.cpp)
target_link_libraries(wave-find-path sc-machine::sc-memory)
```

```cpp
#include <sc-memory/sc_addr.hpp>

ScAddr graph, rrel_arcs, rrel_nodes;
```

If your IDE doesn't integrate well with this solution, try exporting compile commands in CMake:

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

By default, the sc-machine repository contains configuration for launching tests in vscode. If you're developing on vscode, you can use this configuration and extension [C++ TestMate](https://marketplace.visualstudio.com/items?itemName=matepek.vscode-catch2-test-adapter) for debugging code.

## Advanced Usage

### Cross-Compilation

For cross-compilation, use the appropriate CMake toolchain file.

### Editing sc-machine while consuming it as a library

It's almost always problematic to edit a project while also consuming it as a library from another project. Luckily, if you've installed sc-machine in your project using Conan, there is a relatively easy solution to that problem.

```sh
conan editable add <path to sc-machine repo>
conan editable list
# sc-machine of the corresponding version should be in the output
# make changes in the project and run a rebuild
cmake --preset debug-conan
# using a debug build since that would allow us to step into the library's code while debugging
cmake --build --preset debug
```

The same is true for installations that have used GitHub Releases artifacts. Download the sc-machine repo, configure it, run `cmake --build --preset <preset> --target install`. It will copy the built artifacts from their build location to the designated install location. After that, add `sc-machine/build/<Debug|Release>/install` to `CMAKE_PREFIX_PATH` of your project. Re-running `cmake --build --preset <preset> --target install` is enough to propagate changes to the consumer. By these steps, you can seamlessly integrate sc-machine into your project, ensuring that you have access to all necessary components without unnecessary rebuilds.

Relaunch the compilation steps in your project and you should be good to go!

You can find more info in [Conan docs](https://docs.conan.io/2/tutorial/developing_packages/editable_packages.html).

### Falling Back to System Dependencies

If Conan is not used, the build system will attempt to find system-installed dependencies. Ensure that the necessary development packages are installed on your system.

Currently, we require the following packages to be available to CMake at build-time:

- `java`
- `glib2`
- `websocketpp`
  - [`asio`](https://think-async.com/Asio/) as the transitive dependency
- `nlohmann_json`
- `xml2`

You can use convenience scripts provided in our repo (`scripts/install_deps_ubuntu.sh` and `scripts/install_deps_macOS.sh`) to install these dependencies.

### Testing Conan package definition

It is possible to write tests for the Conan packaging using [local package development flow](https://docs.conan.io/2/tutorial/developing_packages/local_package_development_flow.html).

## Troubleshooting

- If you encounter issues with Conan, try clearing the Conan cache: `conan cache clean`
- For CMake configuration issues, check the CMakeCache.txt file in your build directory
- Ensure that your CMake version is compatible with the project requirements

For further assistance, please consult the [project's issue tracker](https://github.com/ostis-ai/sc-machine/issues) or contact the maintainers.
