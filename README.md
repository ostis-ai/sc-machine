## Clone

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
cd sc-machine
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
make
```

## Build knowledge base (from sc-machine/kb folder):
```sh
cd sc-machine/scripts
./build_kb.sh
```

*This repository continues the development of [this sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0.*
