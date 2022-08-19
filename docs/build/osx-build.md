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
Note: by default the binary outputs are saved in the `bin` folder, it is located at the same level as CMake build tree (the `build` folder). If you've generated the build tree in a location different from the root folder of the project, you may need to change further commands in the tutorial to accommodate for changed `bin` folder location. For example, you will need to pass `-b` flag to `build_kb.py` script and start `sc-server` binary from a different folder.
