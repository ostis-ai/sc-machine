
## Clone

```sh
git clone https://github.com/ostis-ai/sc-machine.git
cd sc-machine
```

## Install dependencies

### Setup build packages with brew

```sh
cd scripts
./install_deps_macOS.sh
cd ..
pip3 --user -r requirements.txt
```

## Build

```sh
cd sc-machine
cmake -B build -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
cmake --build build -j$(nproc)
```
