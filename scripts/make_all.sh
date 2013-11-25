cd ../sc-memory
qmake sc-memory.pro
make

cd ../sc-network/cpp
qmake sctp.pro
make

cd ../../tools/builder
qmake builder.pro
make
