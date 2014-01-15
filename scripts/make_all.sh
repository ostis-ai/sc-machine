echo -en '\E[47;31m'"\033[1mBuild sc-machine\033[0m\n"
tput sgr0

cd ..
mkdir build
cd build
cmake ..
make
