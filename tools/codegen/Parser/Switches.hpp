#pragma once

#include <string>

#define DEFINE_SWITCH(name, value)                   \
    const std::string kSwitch##name = value;         \
    const std::string kSwitch##name##Shorthand = ""; \

#define DEFINE_SWITCH_FULL(name, value, shorthand)              \
    const std::string kSwitch##name = value;                    \
    const std::string kSwitch##name##Shorthand = "," shorthand; \

#define SWITCH_OPTION(name)                                 \
    ((kSwitch##name) + (kSwitch##name##Shorthand)).c_str() \

DEFINE_SWITCH_FULL(Help,              "help",           "h");
DEFINE_SWITCH_FULL(TargetName,        "target",		    "t");
DEFINE_SWITCH_FULL(Input,			  "source",		    "i");
DEFINE_SWITCH_FULL(Output,			  "output",		    "o");
DEFINE_SWITCH_FULL(BuildDirectory,    "build_dir",		"b");
DEFINE_SWITCH_FULL(CompilerFlags,     "flags",          "f");
