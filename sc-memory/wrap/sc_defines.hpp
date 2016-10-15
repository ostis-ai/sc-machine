/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_platform.hpp"

#define SC_COMBINE_INTERNAL(v1, v2, v3, v4) v1##v2##v3##v4
#define SC_COMBINE(v1, v2, v3, v4) SC_COMBINE_INTERNAL(v1, v2, v3, v4)

#ifdef __SC_REFLECTION_PARSER__
#define SC_BODY_INTERNAL(...) __attribute__((annotate(#__VA_ARGS__))) void __null_body() {}
#define SC_CLASS_INTERNAL(...) __attribute__((annotate(#__VA_ARGS__))) void __null_meta() {}

#define SC_CLASS(...) SC_CLASS_INTERNAL(__VA_ARGS__)
#define SC_GENERATED_BODY(...) SC_BODY_INTERNAL(GenBody())
#define SC_PROPERTY(...) __attribute__((annotate(#__VA_ARGS__)))
#else

#define SC_GENERATED_BODY_ITEMS(v) SC_COMBINE(ScFileID, _, __LINE__, v)
#define SC_GENERATED_BODY_INIT() \
    public: \
    static bool initGlobal() { return _initStaticInternal(); } \
    private: \
    SC_GENERATED_BODY_ITEMS(_init) \
    SC_GENERATED_BODY_ITEMS(_initStatic) \
    SC_GENERATED_BODY_ITEMS(_decl)


#define SC_CLASS(...)
#define SC_GENERATED_BODY(...)  SC_GENERATED_BODY_INIT()
#define SC_PROPERTY(...)

#define SC_OBJECT_INIT_GLOBAL_CALL(__class) __class::initGlobal();

#endif	// __SC_REFLECTION_PARSER__

// -------------- Deprecation ---------------
#if (SC_COMPILER == SC_COMPILER_MSVC)

//__declspec(deprecated(__Message))// "Update you code to newest API version " #__Version " or later."))
#	define _SC_DEPRECATED_IMPL(__Version, __Message) __declspec(deprecated(__Message "Update you code to newest API version " #__Version " or later."))

#	define PRAGMA_DISABLE_DEPRECATION_WARNINGS \
		__pragma (warning(push)) \
		__pragma (warning(disable:4995)) \
		__pragma (warning(disable:4996))

#	define PRAGMA_ENABLE_DEPRECATION_WARNINGS \
		__pragma (warning(pop))

#elif (SC_COMPILER == SC_COMPILER_CLANG)

#	define _SC_DEPRECATED_IMPL(__Version, __Message) __attribute__((deprecated(__Message "Update you code to newest API version " #__Version " or later.")))

#	define PRAGMA_DISABLE_DEPRECATION_WARNINGS \
		_Pragma ("clang diagnostic push") \
		_Pragma ("clang diagnostic ignored \"-Wdeprecated-declarations\"")

#	define PRAGMA_ENABLE_DEPRECATION_WARNINGS
		_Pragma("clang diagnostic pop")

#elif (SC_COMPILER == SC_COMPILER_GNU)
# define _SC_DEPRECATED_IMPL(__Version, __Message)
#else
#	define _SC_DEPRECATED_IMPL(__Version, __Message)
#endif


#define SC_DEPRECATED(__Version, __Message)	_SC_DEPRECATED_IMPL(__Version, __Message)
