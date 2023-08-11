## Clone

```sh
git clone https://github.com/ostis-ai/sc-machine.git --recursive
cd sc-machine
```

## Install dependencies

Note: you will need [Homebrew](https://docs.brew.sh/Installation) installed and configured on your Mac.

```sh
./scripts/install_deps_macOS.sh
```

## Build

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
cmake --build build -j$(nproc)
```

Note: by default the binary outputs are saved in the `bin` folder, it is located at the same level as CMake build tree (the `build` folder). If you've generated the build tree in a location different from the root folder of the project, you may need to change further commands in the tutorial to accommodate for changed `bin` folder location.
