Clone:

```sh
git clone https://github.com/ostis-ai/sc-machine.git
cd sc-machine
git submodule update --init --recursive
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

Use `cmake` with `-DSC_USE_SANITIZER` option to run build with address or memory sanitizer. As far as `gcc` compiler doesn't support MemorySanitizer you should use `clang` for it. (*example:* `cmake -B build -DSC_USE_SANITIZER=memory -D CMAKE_CXX_COMPILER=clang++ -D CMAKE_C_COMPILER=clang`)

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


*This repository continues the development of [this sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0.*
