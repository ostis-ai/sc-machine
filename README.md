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
cd ..
pip3 install wheel setuptools
pip3 install -r requirements.txt
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

## Build knowledge base:

This repo provides *build_kb.py* script to build and prepare knowledge base (KB).

### Usage
`python3 scripts/build_kb.py <path/to/KB/folder> -o <path/to/output/dir>`
Additionally you can define repo file name (`-f <name>`) or logfile location (`-l <path/to/logfile/dir>`)
To get more information, use `python3 scripts/build_kb.py -h`

*Example:*
```sh
# This command will parse repo.path in current directory
# and create kb.bin and prepared_kb in ../ directory. 
# If errors occured when preparing KB, ./prepare.log will be created

python3 scripts/build_kb.py ./ -o ../ -f repo.path -l ./
```

### repo.path example
```sh
# Comments should start with hashtag as a first character in the line
# Here you can specify path to one or several kb folders
# Paths can be relative
../ims.ostis.kb
```

## Servers

sc-machine provides two network protocols to interact with:

1. **sc-server**: use `./sctipts/run_sc_sever.sh` script to run sc-server
2. **sctp server**: use `./sctipts/run_sctp.sh` script to run sctp server

*This repository continues the development of [this sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0.*
