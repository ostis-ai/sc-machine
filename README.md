Clone:

```sh
git clone https://github.com/ostis-ai/sc-machine.git
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

For running sc-server in authorization server mode, you need a key pair. Current version of http service supports autogeneration of these keys. However you can done this manually with next commands:


Current endpoints for auth service:

3. **127.0.0.1:8090/admin/user** - admin endpoint (need specific type of request for correct work), **for this operation needs access token**:
**POST-request:** add new user,
_**Examples:**_
**POST request-body:** {"name": "alex123", "role: patient", "lang": "ru", "access_token": "..."}

Current credentials to test: login - **admin**, password - **a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3** (SHA256 hash)
All added endpoints respond with specific message codes.
There are eight of them
        **cnt.MSG_ALL_DONE: 0,
        cnt.MSG_INVALID_USERNAME: 1,
        cnt.MSG_INVALID_PASSWORD: 2,
        cnt.MSG_USER_NOT_FOUND: 3,
        cnt.MSG_USER_IS_IN_BASE: 4**

Required additional packages:
**json, jwt, tornado**

Open URL http://localhost:8090/ in web browser:
![](https://i.imgur.com/wibISSV.png)


*This repository continues the development of [this sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0.*
