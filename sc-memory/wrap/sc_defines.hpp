/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#define SC_COMBINE_BODY_ID_INTERNAL(v1, v2) v1##v2
#define SC_COMBINE_BODY_ID(v1, v2) SC_COMBINE_BODY_ID_INTERNAL(v1, v2)

#ifdef __SC_REFLECTION_PARSER__

#define SC_CLASS(...) 
#define SC_GENERATED_BODY(...) 
#define SC_PROPERTY(...) __attribute__((annotate(#__VA_ARGS__)))

#else

#define SC_CLASS(...)
#define SC_GENERATED_BODY(...) SC_COMBINE_BODY_ID(ScFileID, _body) 
#define SC_PROPERTY(...)

#endif