[![codecov](https://codecov.io/gh/ostis-ai/sc-machine/branch/main/graph/badge.svg?token=WU8O9Z1DNL)](https://codecov.io/gh/ostis-ai/sc-machine)

## Documentation: [GitHub Pages](https://ostis-ai.github.io/sc-machine)

## Clone:

```sh
git clone https://github.com/ostis-ai/sc-machine.git
cd sc-machine
git submodule update --init --recursive
```

## Install dependencies

### Debian-based (Ubuntu, Debian, Mint)

```sh
cd scripts
./install_deps_ubuntu.sh
```

### macOS
```sh
cd scripts
./install_deps_macOS.sh
cd ..
pip3 install wheel setuptools
pip3 install -r requirements.txt
```
Please note: you should add Qt5 and LLVM to `PATH` variable. To do this, after installing dependencies execute the following commands (considering you use `zsh` as your shell):
```sh
echo 'export PATH="'$HOMEBREW_PREFIX'/opt/qt@5/bin:$PATH"' >> ~/.zshrc
echo 'export PATH="'$HOMEBREW_PREFIX'/opt/llvm/bin:$PATH"' >> ~/.zshrc
```

## Build sc-machine
```sh
cd sc-machine/scripts
./make_all.sh #You can also pass all CMake generation arguments there
```
or, alternatively (requires CMake 3.13+)
```sh
cd sc-machine
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc) #-j flag for paralleled build process
```
Additional CMake flags can be used to build tests, format code or analyze memory leaks, check [our build docs](docs/build/cmake-flags.md) for more info.

## Build knowledge base

This repo provides *build_kb.py* script to prepare and build knowledge base (KB). Note that you will need to run a server with a path to a compiled KB in the next step.

### Usage example
```sh
cd sc-machine
python3 scripts/build_kb.py ../ims.ostis.kb -c config/sc-machine.ini
```

Default config file saves compiled KB one level above sc-machine, `sc-machine/..`

Alternatively, you can use `build_kb.py` without a config file to define output path and other settings manually:
```sh
cd sc-machine
build_kb.py [-o OUTPUT_PATH] [-l ERRORS_FILE_PATH] REPO_FILE
```
To get more information, use `python3 scripts/build_kb.py -h`

**Note: flags have higher priority than config file.**

### repo.path example
```sh
# Comments should start with hashtag as a first character in the line
# Here you can specify path to one or several kb folders
# Paths can be relative
../ims.ostis.kb
#../custom_kb
```

## Servers

sc-machine provides two network protocols to interact with:
1. **sc-server**: use `python3 scripts/run_sc_sever.py -c config/sc-machine.ini` to run sc-server with the default config file.
  To get more information use `python3 scripts/run_sc_server.py -h`
2. **sctp server**: use `python3 scripts/run_sctp.py -c config/sc-machine.ini` to run sctp server.
  To get more information use `python3 scripts/run_sctp.py -h`

## Config

This repository provides a default configuration for sc-machine. To customize *sc-machine* usage you can create your own config file. You can check docs at [docs/other/config.md](docs/other/config.md)

*This repository continues the development of [this sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0.*
