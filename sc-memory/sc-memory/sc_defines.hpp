/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-core/sc-store/sc_defines.h"

#define SC_COMBINE_INTERNAL(v1, v2, v3, v4) v1##v2##v3##v4
#define SC_COMBINE(v1, v2, v3, v4) SC_COMBINE_INTERNAL(v1, v2, v3, v4)

#ifdef __SC_REFLECTION_PARSER__
#  define SC_BODY_INTERNAL(...) \
    __attribute__((annotate(#__VA_ARGS__))) void __null_body() \
    { \
    }
#  define SC_CLASS_INTERNAL(...) \
    __attribute__((annotate(#__VA_ARGS__))) void __null_meta() \
    { \
    }

#  define SC_CLASS(...) SC_CLASS_INTERNAL(__VA_ARGS__)
#  define SC_GENERATED_BODY(...) SC_BODY_INTERNAL(GenBody())
#  define SC_PROPERTY(...) __attribute__((annotate(#  __VA_ARGS__)))
#else

#  define SC_GENERATED_BODY_ITEMS(v) SC_COMBINE(ScFileID, _, __LINE__, v)
#  define SC_GENERATED_BODY_INIT() \
  public: \
    static bool InitGlobal() \
    { \
      return _InitStaticInternal(); \
    } \
\
  private: \
    SC_GENERATED_BODY_ITEMS(_init) \
    SC_GENERATED_BODY_ITEMS(_initStatic) \
    SC_GENERATED_BODY_ITEMS(_decl)

#  define SC_CLASS(...)
#  define SC_GENERATED_BODY(...) SC_GENERATED_BODY_INIT()
#  define SC_PROPERTY(...)

#  define SC_OBJECT_INIT_GLOBAL_CALL(__class) __class::InitGlobal();

// for autocompletion
#  define Keynode(__X)
#  define ForceCreate(__X)
#  define CmdClass
#  define Agent

// utils
#  define SC_UNUSED(__X) (void)__X

#endif  // __SC_REFLECTION_PARSER__
