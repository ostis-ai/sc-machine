APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)

sudo apt-get update
sudo apt-get install -y \
                     qt5-qmake qtbase5-dev libglib2.0-dev libboost-system-dev \
                     libboost-filesystem-dev libboost-program-options-dev \
                     make cmake antlr gcc g++ llvm \
                     libcurl4-openssl-dev libclang-dev libboost-python-dev \
                     python3-dev python3 python3-pip python3-setuptools clang-format ccache

pip3 install wheel setuptools
pip3 install -r ${APP_ROOT_PATH}/requirements.txt
