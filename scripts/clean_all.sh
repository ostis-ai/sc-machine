echo -en '\E[47;31m'"\033[1mClean sc-machine\033[0m\n"
tput sgr0
cd ../
make clean

echo -en '\E[47;31m'"\033[1mClean KPM modules\033[0m\n"
tput sgr0
cd sc-kpm
make clean

echo -en '\E[47;31m'"\033[1mClean sctp server\033[0m\n"
tput sgr0
cd ../sc-network/cpp
make clean

echo -en '\E[47;31m'"\033[1mClean builder\033[0m\n"
tput sgr0
cd ../../tools/builder
make clean
~                       
