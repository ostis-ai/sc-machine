cd ..
mkdir -p build
cd build
cmake .. $@ -DSC_FILE_MEMORY=Dictionary
make -j$(nproc)
