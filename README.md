# sc-machine
Software implementation of sc-machine

# Build
[![Build Status](https://travis-ci.org/ostis-dev/sc-machine.svg?branch=master)](https://travis-ci.org/ostis-dev/sc-machine)

Build on ubuntu:
```sh
git clone https://github.com/deniskoronchik/sc-machine.git
cd sc-machine
./scripts/install_deps_ubuntu.sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
make
```
