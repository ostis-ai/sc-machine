**Chat:**

[![Join the chat at https://gitter.im/ostis-dev/sc-machine](https://badges.gitter.im/ostis-dev/sc-machine.svg)](https://gitter.im/ostis-dev/sc-machine?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

**Documentation**
[http://ostis-dev.github.io/sc-machine/](http://ostis-dev.github.io/sc-machine/)

**Master:** 
[![Build Status](https://travis-ci.com/ostis-dev/sc-machine.svg?branch=master)](https://travis-ci.com/ostis-dev/sc-machine)

Clone:

```sh
git clone https://github.com/ostis-dev/sc-machine.git
cd sc-machine
```

Install dependencies:

```sh
cd scripts
./install_deps_ubuntu.sh
cd ..
pip3 install -r requirements.txt
```

Build sc-machine:
```sh
cd sc-machine
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
make
```

Build knowledge base (from sc-machine/kb folder):
```sh
cd sc-machine/scripts
./build_kb.sh
```

Build web interface:
- Install yarn
```sh
sudo apt remove cmdtest
curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | sudo apt-key add -
echo "deb https://dl.yarnpkg.com/debian/ stable main" | sudo tee /etc/apt/sources.list.d/yarn.list
sudo apt update
sudo apt install yarn
```
- Build
```sh
cd sc-machine/web/client
yarn && yarn run webpack-dev
```

Run sc-server:
```sh
cd sc-machine/scripts
./run_sc_server.sh
```

Open URL http://localhost:8090/ in web browser:
![](https://i.imgur.com/wibISSV.png)