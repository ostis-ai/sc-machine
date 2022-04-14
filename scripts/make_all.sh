cd ..
cmake -B build $@
cmake --build build -j$(nproc)
