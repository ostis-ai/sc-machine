## Building tests:
```sh
cd sc-machine
cmake -B build -DSC_BUILD_TESTS=ON -DSC_AUTO_TEST=ON -DSC_KPM_SCP=OFF
cmake --build build -j$(nproc)
```

Additionally you can use `-DSC_BUILD_BENCH=ON` flag to build performance tests


## Building with sanitizers
Use `cmake` with `-DSC_USE_SANITIZER=memory` or `-DSC_USE_SANITIZER=address` option to run build with memory or address sanitizer. 
**Note: sanitizers are only supported by `clang` compiler** 
### Example:
```sh
cd sc-machine
cmake -B build -DSC_USE_SANITIZER=memory -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
cmake --build build
```
## Code formatting with CLangFormat:

To check code with CLangFormat run:
```sh
cd sc-machine
cmake -B build -DSC_CLANG_FORMAT_CODE=ON
cmake --build build --target clangformat_check
```

or
```sh
cd sc-machine
./scripts/ci/check-formatting.sh
```

To format code with CLangFormat run:
```sh
cd sc-machine
cmake -B build -DSC_CLANG_FORMAT_CODE=ON
cmake --build build --target clangformat
```
