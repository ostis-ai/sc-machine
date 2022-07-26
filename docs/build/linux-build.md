This instruction is intended to be used on Debian-based distros. For other distros, skip [Install dependencies](#install-dependencies) section and make sure you install all the required packages manually.

## Clone

```sh
git clone https://github.com/ostis-ai/sc-machine.git
cd sc-machine
```

## Install dependencies

```sh
cd scripts
./install_deps_ubuntu.sh --dev
```

## Build

```sh
cd sc-machine
cmake -B build -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
cmake --build build -j$(nproc)
```
Alternatively, you can use a script:
```sh
cd sc-machine/scripts
./make_all.sh #You can also pass all CMake generation arguments there
```
