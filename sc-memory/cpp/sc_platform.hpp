/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

// Determine compiler
#define SC_COMPILER			0
#define SC_COMPILER_GNU		1
#define SC_COMPILER_CLANG	2
#define SC_COMPILER_MSVC	3

#if defined(__clang__)
#	undef SC_COMPILER
#	define SC_COMPILER SC_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)	// after clang, possible it also has the same defines (need to research)
#	undef SC_COMPILER
#	define SC_COMPILER SC_COMPILER_GNU
#elif defined(_MSC_VER)
#	undef SC_COMPILER
#	define SC_COMPILER SC_COMPILER_MSVC
#else
#	error "Unsupported compiler"
#endif


// Determine platform
#define SC_PLATFORM				0
#define SC_PLATFORM_WIN32		1
#define SC_PLATFORM_LINUX		2
#define SC_PLATFORM_MAC			3
#define SC_PLATFORM_IOS			4
#define SC_PLATFORM_ANDROID		5


#if (defined(__WIN32__) || defined(_WIN32)) && !defined(__ANDROID__)
#	undef SC_PLATFORM
#	define SC_PLATFORM SC_PLATFORM_WIN32
#elif defined(__APPLE_CC__) 
#	undef SC_PLATFORM
#	define SC_PLATFORM SC_PLATFORM_MAC
/* TODO: add iOS support
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 60000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 60000
*/
#elif defined(__ANDROID__)
#	undef SC_PLATFORM
#	define SC_PLATFORM SC_PLATFORM_ANDROID
#else	// let all other platform right now is a linux platforms
#	undef SC_PLATFORM
#	define SC_PLATFORM SC_PLATFORM_LINUX
#endif
