/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_defines_h_
#define _sc_defines_h_

#ifdef SC_DEBUG
# define SC_DEBUG_MODE 1
#else
# define SC_DEBUG_MODE 0
#endif

/*! Bound empty slot serach
 *
 * Can be used just with USE_SEGMENT_EMPTY_SLOT_BUFFER = 0
 */
#define BOUND_EMPTY_SLOT_SEARCH 0

//! Enable network scaling
#define USE_NETWORK_SCALE 0

#define MAX_PATH_LENGTH 1024

#define SC_CONCURRENCY_LEVEL   32   // max number of independent threads that can work in parallel with memory
#define SC_SEGMENT_CACHE_SIZE  32   // size of segments cache

#if defined (SC_MEMORY_SELF_BUILD)
    #if defined (SC_PLATFORM_WIN)
        #define _SC_EXTERN __declspec(dllexport) 
    #else
        #define _SC_EXTERN
    #endif
#else
    #if defined (SC_PLATFORM_WIN)
        #define _SC_EXTERN __declspec(dllimport)
    #else
        #define _SC_EXTERN
    #endif
#endif

#if defined (SC_PLATFORM_WIN)
#   if defined(__cplusplus)
#       define _SC_EXT_EXTERN extern "C" __declspec(dllexport)
#   else
#       define _SC_EXT_EXTERN __declspec(dllexport)
#   endif
#else
    #define _SC_EXT_EXTERN
#endif


#endif // _sc_defines_h_
