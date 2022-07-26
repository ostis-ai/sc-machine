## Clone

```sh
git clone https://github.com/ostis-ai/sc-machine.git
cd sc-machine
```

## Install dependencies
```sh
cd scripts
./install_deps_macOS.sh
cd ..
pip3 --user -r requirements.txt
```

Please note: you should add LLVM to `PATH` variable. To do this, after installing dependencies execute the following commands (considering you use `zsh` as your shell):
```sh
echo 'export PATH="'$HOMEBREW_PREFIX'/opt/llvm/bin:$PATH"' >> ~/.zshrc
```

## Build

```sh
cd sc-machine
cmake -B build -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
cmake --build build -j$(nproc)
```
