In general, [Google's coding standard](https://google.github.io/styleguide/cppguide.html) is used, and we strongly 
encourage to read it.

## Code Style Guide

All code should conform to C++17 standard. Here's a summary of the key rules specified in the configuration:

### Includes

* We use `.cpp` and `.hpp` files, not `.cc` and `.h` (`.c` and `.h` are used for C code), in UTF-8 encoding.
* File names are lowercase with underscores, like `sc_memory.cpp`.
* Directory names are lowercase with hyphen, like `sc-memory/`.
* We use `#pragma once` instead of the `#define` Guard in header files.
* Include categories are specified with priorities, such as `gtest/gmock`, `sc-memory`, `sc_*` and others.
* Includes are sorted and grouped by directory, there should be newlines between different directories.
* Order of directories in includes: `current_dir/current_file.hpp`, other includes from the same dir, includes from other dirs sorted by name.

```cpp
#include "../test.hpp"

#include "hash/hmac.h"
#include "hash/sha256.h"

#include "sc-memory/cpp/sc_wait.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include <curl/curl.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
```

* Don't use `using namespace std` or other namespaces globally. You can use them locally in test cpp files or functions.

### Indentation and Spacing

* The code should use spaces for indentation. The indentation width is set to 2 spaces. The code should not use tabs.

```cpp
// It is correct style.
sc_uint32 ExampleFunc() 
{
  if (condition) 
  {
    // Code block
  } 
  else 
  {
    // Code block
  }
  
  return 0;
}

// It is incorrect style.
sc_uint32 ExampleFunc() 
{
    if (condition) 
    {
        // Code block
    } 
    else 
    {
        // Code block
    }
  
    return 0;
}
```

* Spaces before the opening parenthesis in control statements.

```cpp
// It is correct style.
void ExampleFunction(sc_uint32 x) 
{
  if (x > 0) 
  {
    // Code block
  }

  for (sc_uint32 i = 0; i < x; ++i) 
  {
    // Code block
  }

  while (x > 10) 
  {
    // Code block
  }
}

// It is incorrect style.
void ExampleFunction(sc_uint32 x) 
{
  if(x > 0) 
  {
    // Code block
  }

  for(sc_uint32 i = 0; i < x; ++i) 
  {
    // Code block
  }

  while(x > 10) 
  {
    // Code block
  }
}
```

* Spaces before the colon in constructor initializer lists.

```cpp
// It is correct style.
class MyClass {
public:
  // Constructor with initializer list
  MyClass(sc_uint32 x, sc_uint32 y)
    : memberX(x)
    , memberY(y) {
    // Constructor body
  }

private:
  sc_uint32 memberX;
  sc_uint32 memberY;
};

```

* Spaces should be inserted before assignment operators and around pointer and reference qualifiers.

```cpp
// It is correct style.
sc_uint32 x = 10;
sc_uint32 * ptr = &x;
sc_uint32 const * constPtr = &x;
sc_uint32 & ref = x;

// It is incorrect style.
sc_uint32 x = 10;
sc_uint32* ptr = &x;
const sc_uint32 *constPtr = &x;
sc_uint32& ref = x;
```

* Use right-to-left order for variables/params.

```cpp
// It is correct style.
ScAddr const & addr; // reference to the const `ScAddr`.
```

### Whitespace

* Remove extra whitespaces at the end of lines.
* Spaces in empty blocks and parentheses should be omitted.
* Trailing commas should be added in wrapped situations.

```cpp
// It is correct style.
std::vector<sc_uint32> numbers = {
    1,
    2,
    3,  // Trailing comma added
};
```

### Brace Style

* Braces for functions, classes, namespaces, etc., should be on a new line.

```cpp
// It is correct style.
void ExampleFunction() 
{
  // Code block
}

class ExampleClass 
{
public:
  // Class members
};

namespace exampleNamespace 
{
  // Namespace contents
}

// It is incorrect style.
void ExampleFunction() {
  // Code block
}

class ExampleClass {
public:
  // Class members
};

namespace exampleNamespace {
  // Namespace contents
}
```

* Empty functions, classes, and namespaces should be split into separate lines.

```cpp
// It is correct style.
void ExampleFunction() 
{
}

class ExampleClass 
{
};

namespace exampleNamespace 
{
}
```

* Braces for control statements always on a new line.

```cpp
// It is correct style.
if (condition) 
{
  // Code block
} 
else 
{
  // Code block
}

for (sc_uint32 i = 0; i < 10; ++i) 
{
  // Code block
}

// It is incorrect style.
```

* Always break after a multiline string.

```cpp
// It is correct style.
std::string multilineString = "This is a long string that "
                             "spans multiple lines.";
```

* Always break functions declarations and calls.

```cpp
// It is correct style.
sc_uint32 ExampleFunction(
    sc_uint32 argument1, 
    sc_uint32 argument2, 
    sc_uint32 argument3, 
    sc_uint32 * result, 
    sc_uint32 * delta)
    
// It is incorrect style.
sc_uint32 ExampleFunction(
    sc_uint32 argument1, sc_uint32 argument2, sc_uint32 argument3, 
    sc_uint32 * result, sc_uint32 * delta)
```

### Line Length and Wrapping

* The maximum column limit is set to 120 characters.

* Break binary operators before the operator.

```cpp
// It is correct style.
sc_uint32 const result = longVariableName
    + anotherLongVariableName
    + yetAnotherLongVariableName;

// It is incorrect style.
sc_uint32 const result = longVariableName +
    anotherLongVariableName +
    yetAnotherLongVariableName;
```

* Break before ternary operators.

```cpp
// It is correct style.
sc_uint32 const value = condition
    ? trueValue
    : falseValue;

// It is incorrect style.
sc_uint32 const value = condition ?
    trueValue :
    falseValue;
```

* Break before braces in custom scenarios.

```cpp
// It is correct style.
if (condition) 
{
  // Code block
} 
else 
{
  // Code block
}

// It is incorrect style.
if (condition) {
  // Code block
} 
else {
  // Code block
}
```

* In one line `if`, `for`, `while` we do not use brackets. If one line `for` or `while` is combined with one line `if`, do use brackets for cycle.

```cpp
for (ScAddr const & addr : addrList)
  ctx.EraseElement(addr);

...

for (ScAddr const & addr : addrList)
{
  if (addr.IsValid())
    return SC_TRUE;
}
```

### Naming

#### C Naming

* Macros and C-style enums must be named in UPPER_CASE, and enum values must be prefixed with a capitalized enum name.
* All other objects must be named in snake_case.

#### C++ Naming

* Underscores are allowed only in prefixes for member variables and namespace names, like `int m_keynodeAddr; namespace sc_utils`.
* We use `using` keyword instead of `typedef`.
* Compile-time constants must be named in camelCase, starting with a lower-case `k`, e.g. `kCompileTimeConstant` 
and marked as `constexpr` when possible.
* Values of enum classes must be named in CamelCase, e.g. `enum class Color { Red, Green, LightBlue };`.

### Comments

* Doxygen-style comments can be used.

## Check code style

Most of our coding style is specified in a configuration file for [ClangFormat](http://clang.llvm.org/docs/ClangFormat.html).
To automatically format a file, install `clang-format` and run:

```sh
cd sc-machine
clang-format -i file.cpp file.hpp other_file.cpp
```

To check all format code use script `check_formatting.sh`.

```sh
cd sc-machine
./scripts/clang/check_formatting.sh
```

Or use script `format_code.sh` to reformat all code.

```sh
cd sc-machine
./scripts/clang/format_code.sh
```

## Tips and Hints

- Code should compile without warnings! Just deprecated warning could be present for a short time. But if you see them,
  then try to fix it in a separate commit.
- If you see outdated code which can be improved - DO IT NOW (but in the separate pull request).
- Your code should work at least on: Ubuntu 20.04 and 22.04; Debian 11; macOS platforms.
- Your code should compile well with the last gcc and clang.
- Try to avoid using any new 3rd party library if it is not fully tested and supported on supported platforms.
- Cover your code with unit tests.
- If you don't have enough time to make it right, leave a `// TODO(DeveloperName): need to fix it` comment and [make 
issue on GitHub](https://github.com/ostis-ai/sc-machine/issues/new/choose).
