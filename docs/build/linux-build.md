This instruction is intended to be used on Debian-based distros. For other distros, skip [Install dependencies](#install-dependencies) section and make sure you install all the required packages manually.

## Clone

```sh
git clone https://github.com/ostis-ai/sc-machine.git --recursive
cd sc-machine
```

## Install dependencies

```sh
./scripts/install_deps_ubuntu.sh --dev
```

## Build

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
cmake --build build -j$(nproc)
```

Alternatively, you can use a script:
```sh
./scripts/build_sc_machine.sh #You can also pass -h to get help menu
```
Note: by default the binary outputs are saved in the `bin` folder, it is located at the same level as CMake build tree (the `build` folder). If you've generated the build tree in a location different from the root folder of the project, you may need to change further commands in the tutorial to accommodate for changed `bin` folder location.
