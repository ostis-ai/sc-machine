## Build cache

This project uses ccache automatically if it's available in the system. To disable this, use flag `-DAUTO_CCACHE=OFF`.

## Building tests

```sh
cmake --preset <configure-preset> -DSC_BUILD_TESTS=ON
cmake --build --preset <build-preset>
```

Additionally you can use `-DSC_BUILD_BENCH=ON` flag to build performance tests

## Building sc-machine with sanitizers

Use `cmake` with `-DSC_USE_SANITIZER=memory` or `-DSC_USE_SANITIZER=address` option to run build with memory or address sanitizer. 

**Note: sanitizers are only supported by `clang` compiler** 

### Example

```sh
cmake --preset <configure-preset> -DSC_USE_SANITIZER=memory -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
cmake --build --preset <build-preset>
```

## Code formatting with CLangFormat

To check code with CLangFormat run:
```sh
cmake --preset release-with-tests -DSC_CLANG_FORMAT_CODE=ON
cmake --build --preset release --target clangformat_check
```

or
```sh
./scripts/clang/check_formatting.sh
```

To format code with CLangFormat run:
```sh
cmake --preset release-with-tests -DSC_CLANG_FORMAT_CODE=ON
cmake --build --preset release --target clangformat
```

or
```sh
./scripts/clang/format_code.sh
```
