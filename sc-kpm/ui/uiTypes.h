/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _ui_types_h_
#define _ui_types_h_

extern "C"
{
#include "sc_memory_headers.h"
}

#include <string>
#include <sstream>
#include <map>
#include <list>
#include <vector>
#include <assert.h>
#include <stdint.h>

typedef std::string String;
typedef std::stringstream StringStream;

typedef std::list<sc_addr> tScAddrList;
typedef std::vector<sc_addr> tScAddrVector;
typedef std::map<sc_addr, sc_type> tScAddrToScTypeMap;
typedef std::map<sc_addr, sc_addr> tScAddrToScAddrMap;
typedef std::pair<sc_addr, sc_addr> tScAddrPair;
typedef std::list< tScAddrPair > tScAddrPairList;
typedef std::map<String, String> tStringStringMap;

// --- operators ---
bool operator < (const sc_addr &addr1, const sc_addr &addr2);
bool operator == (const sc_addr &addr1, const sc_addr &addr2);
bool operator != (const sc_addr &addr1, const sc_addr &addr2);

extern sc_memory_context * s_default_ctx;

#endif
