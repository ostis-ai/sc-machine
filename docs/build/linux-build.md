Build with Ubuntu:
```sh
git clone https://github.com/ostis-dev/sc-machine.git
cd sc-machine
./scripts/install_deps_ubuntu.sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
make
```
