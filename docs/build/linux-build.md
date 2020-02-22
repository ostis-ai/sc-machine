This instruction made for Ubuntu.

## Clone

```sh
git clone https://github.com/ostis-dev/sc-machine.git
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
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
make
```
