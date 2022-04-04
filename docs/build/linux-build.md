This instruction made for Ubuntu.

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
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
make
```

To build tests run:
```shell
cd sc-machine
mkdir build
cd build
cmake .. -DSC_BUILD_TESTS=ON -DSC_AUTO_TEST=ON -DSC_KPM_SCP=OFF
make
```

or:
```shell
cd sc-machine
./scripts/ci/make-tests.sh
```

## CLangFormat code check

To check code with CLangFormat run:
```shell
cd sc-machine
mkdir build
cd build
cmake .. -DSC_CLANG_FORMAT_CODE=ON
make clangformat_check
```

To format code with CLangFormat run:
```shell
cd sc-machine
mkdir build
cd build
cmake .. -DSC_CLANG_FORMAT_CODE=ON
make clangformat
```
or
```shell
cd sc-machine
./scripts/ci/check-formatting.sh
```