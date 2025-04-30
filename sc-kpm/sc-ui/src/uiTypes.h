/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _ui_types_h_
#define _ui_types_h_

#include <string>
#include <sstream>
#include <map>
#include <list>
#include <vector>
#include <unordered_set>
#include <assert.h>
#include <stdint.h>

extern "C"
{
#include <sc-core/sc_memory_headers.h>
}

typedef std::string String;
typedef std::stringstream StringStream;

struct _sc_addr_hash
{
  auto operator()(sc_addr const & addr) const -> size_t
  {
    return SC_ADDR_LOCAL_TO_INT(addr);
  }
};

#include <nlohmann/json.hpp>

typedef std::list<sc_addr> tScAddrList;
typedef std::vector<sc_addr> tScAddrVector;
typedef std::map<sc_addr, sc_type> tScAddrToScTypeMap;
typedef std::map<sc_addr, sc_addr> tScAddrToScAddrMap;
typedef std::map<sc_addr, String> tScAddrToStringMap;
typedef std::pair<sc_addr, sc_addr> tScAddrPair;
typedef std::list<tScAddrPair> tScAddrPairList;
typedef std::map<String, String> tStringStringMap;
typedef std::unordered_set<sc_addr, _sc_addr_hash> tScAddrSet;

typedef nlohmann::json ScJson;

// --- operators ---
bool operator<(sc_addr const & addr1, sc_addr const & addr2);
bool operator==(sc_addr const & addr1, sc_addr const & addr2);
bool operator!=(sc_addr const & addr1, sc_addr const & addr2);

extern sc_memory_context * s_default_ctx;

#endif
