[![codecov](https://codecov.io/gh/ostis-ai/sc-machine/branch/main/graph/badge.svg?token=WU8O9Z1DNL)](https://codecov.io/gh/ostis-ai/sc-machine)

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
pip3 install -r requirements.txt
```

### macOS
```sh
cd scripts
./install_deps_macOS.sh
cd ..
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
or, alternatively
```sh
cd sc-machine
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc) #-j flag for paralleled build process
```

## Build knowledge base (from sc-machine/kb folder):
Use `cmake` with `-DSC_USE_SANITIZER` option to run build with address or memory sanitizer.(*example:* `cmake -B build -DSC_USE_SANITIZER=memory -D CMAKE_CXX_COMPILER=clang++ -D CMAKE_C_COMPILER=clang`)

**Note: sanitizers are supported only by clang compiler**

```sh
cd sc-machine/scripts
./build_kb.sh
```

*This repository continues the development of [this sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0.*
