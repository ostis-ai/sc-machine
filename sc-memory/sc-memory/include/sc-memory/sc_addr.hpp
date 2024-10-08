/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <cstdint>
#include <list>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>

extern "C"
{
#include <sc-core/sc_types.h>
}

using ScRealAddr = sc_addr;

class _SC_EXTERN ScAddr
{
  friend class ScMemoryContext;
  friend class ScTemplateItem;

public:
  using HashType = sc_addr_hash;

  static ScAddr const Empty;

  ScAddr();
  ScAddr(sc_addr const & addr);
  ScAddr(ScAddr::HashType const & hash);

  bool IsValid() const;
  void Reset();

  bool operator==(ScAddr const & other) const;
  bool operator!=(ScAddr const & other) const;
  ScRealAddr const & operator*() const;
  HashType Hash() const;

  ScRealAddr const & GetRealAddr() const;

protected:
  ScRealAddr m_realAddr;
};

struct _SC_EXTERN RealAddrLessFunc
{
  bool operator()(ScRealAddr const & a, ScRealAddr const & b) const
  {
    if (a.seg < b.seg)
      return true;

    if (a.seg > b.seg)
      return false;

    return (a.offset < b.offset);
  }
};

struct _SC_EXTERN ScAddrLessFunc
{
  bool operator()(ScAddr const & a, ScAddr const & b) const
  {
    return RealAddrLessFunc()(*a, *b);
  }
};

struct ScAddrHashFunc
{
  ScAddr::HashType operator()(ScAddr const & addr) const
  {
    return SC_ADDR_LOCAL_TO_INT(*addr);
  }
};

using ScAddrVector = std::vector<ScAddr>;
using ScAddrList = std::list<ScAddr>;
using ScAddrStack = std::stack<ScAddr>;
using ScAddrQueue = std::queue<ScAddr>;
using ScAddrSet = std::set<ScAddr, ScAddrLessFunc>;
using ScAddrUnorderedSet = std::unordered_set<ScAddr, ScAddrHashFunc>;
template <typename Value>
using ScAddrToValueUnorderedMap = std::unordered_map<ScAddr, Value, ScAddrHashFunc>;
