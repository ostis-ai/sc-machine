**Chat:**

[![Join the chat at https://gitter.im/ostis-dev/sc-machine](https://badges.gitter.im/ostis-dev/sc-machine.svg)](https://gitter.im/ostis-dev/sc-machine?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

# Build status
**Master:** 

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
