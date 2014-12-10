/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
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
