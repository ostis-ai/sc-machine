## Setup Java

Use package: https://support.apple.com/kb/dl1572?locale=en_US

## Clone

```sh
git clone https://github.com/ostis-dev/sc-machine.git
cd sc-machine
```

### Install dependencies

```sh
cd scripts
./install_deps_osx.sh
cd ..
pip3 --user -r requirements.txt
```

## Build

```sh
cd sc-machine
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
make
```
