
## Clone

```sh
git clone https://github.com/ostis-ai/sc-machine.git
cd sc-machine
```

## Install dependencies

### Setup Java

Use package: https://support.apple.com/kb/dl1572?locale=en_US

### Setup build packages with brew

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
