APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)

sudo add-apt-repository universe
sudo apt-get update
sudo apt-get install -y \
                     libglib2.0-dev libboost-system-dev \
                     libboost-filesystem-dev libboost-program-options-dev \
                     make cmake antlr gcc g++ \
                     libcurl4-openssl-dev libclang-dev \
                     libwebsocketpp-dev nlohmann-json3-dev \
                     python3-dev python3 python3-pip python3-setuptools clang-format ccache

pip3 install wheel setuptools
pip3 install -r ${APP_ROOT_PATH}/requirements.txt
