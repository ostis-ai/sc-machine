## Setup Java

Use package: https://support.apple.com/kb/dl1572?locale=en_US

## Installation

* Run `scripts/install_deps_osx.sh` script
* 
```sh
cd sc-machine
mkdir build
cd build
cmake .. -DSC_KPM_SCP=OFF -DCMAKE_BUILD_TYPE=Release
make -j4
``` 