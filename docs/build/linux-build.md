This instruction is intended to be used on Ubuntu.

## Clone

```sh
git clone https://github.com/ostis-ai/sc-machine.git
cd sc-machine
```

### Install dependencies

```sh
cd scripts
./install_deps_ubuntu.sh
cd ..
pip3 --user -r requirements.txt
```

## Build

```sh
cd sc-machine
cmake -B build -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
cmake --build build -j$(nproc)
```
Alternatively, you can use a script:
```sh
cd sc-machine
cd scripts
./make_all.sh #You can also pass all CMake generation arguments there
```
