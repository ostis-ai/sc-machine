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

## Build knowledge base

This repo provides *build_kb.py* script to build and prepare knowledge base (KB).

### Usage
`python3 scripts/build_kb.py -i <path/to/KB/folder> -o <path/to/output/dir>`
Additionally you can define repo file name (`-f <name>`), logfile location (`-l <path/to/logfile/dir>`) or config file path
(`-c <path/to/config/file>`).
To get more information, use `python3 scripts/build_kb.py -h`

**Note: flags have higher priority than config file.**

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

## Config

To customize *sc-machine* usage you can create your own config file.

Allowed options:
1. [Network]
  - `Port` - port for redis connection
2. [Repo]
  - `Source` - directory containing repo file
  - `Log` - directory where error log file will be stored
  - `Path` - path to compiled knowledge base folder
  - `SavePeriod` - time before KB save
3. [Extensions]
  - `Directory` - directory with sc-machine extensions
4. [Stat]
  - `UpdatePeriod` - time before KB update
  - `Path` - path to folder with sctp statistic
5. [memory]
  - `max_loaded_segments` - number of maximum loaded segments from kb
6. [filememory]
  - `engine` - engine used for reading KB (only redis supported!)
7. [kpm]
  - `threads` - maximum number of threads
8. [redis]
  - `host` - host of redis 
9. [python]
  - `modules_path` - path to `sc-kpm/sc-python/services`
10. [debug]
  - `is_debug` - debug mode enable (True|False)

*Config file example*:
```ini
[Network]
Port = 55770
[Repo]
Source = ./
Logfile = ./
Path = ../kb.bin
SavePeriod = 3600
[Extensions]
Directory = ../bin/extensions
[Stat]
UpdatePeriod = 1800
Path = /tmp/sctp_stat
##### sc-memory
[memory]
max_loaded_segments = 1000

[filememory]
engine = redis

[kpm]
max_threads = 32

[redis]
host = 127.0.0.1

[python]
modules_path = /sc-machine/sc-kpm/sc-python/services

[debug]
is_debug = True

```

*This repository continues the development of [this sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0.*
