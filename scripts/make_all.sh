echo -en '\E[47;31m'"\033[1mBuild sc-memory\033[0m\n"
tput sgr0
cd ../sc-memory
qmake sc-memory.pro
make

echo -en '\E[47;31m'"\033[1mBuild KPM modules\033[0m\n"
tput sgr0
cd ../sc-kpm
qmake sc_kpm.pro
make

echo -en '\E[47;31m'"\033[1mBuild sctp server\033[0m\n"
tput sgr0
cd ../sc-network/cpp
qmake sctp.pro
make

echo -en '\E[47;31m'"\033[1mBuild builder\033[0m\n"
tput sgr0
cd ../../tools/builder
qmake builder.pro
make
