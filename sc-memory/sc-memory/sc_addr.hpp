/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <cstdint>
#include <list>
#include <vector>

extern "C"
{
#include "sc-core/sc-store/sc_types.h"
}

using ScRealAddr = sc_addr;

class _SC_EXTERN ScAddr
{
  friend class ScMemoryContext;

  template <typename ParamType1, typename ParamType2, typename ParamType3>
  friend class TIterator3;
  template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
  friend class TIterator5;

public:
  using HashType = uint64_t;

  static ScAddr const Empty;

  ScAddr();
  ScAddr(sc_addr const & addr);
  explicit ScAddr(HashType const & hash);

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

using ScAddrVector = std::vector<ScAddr>;
using ScAddrList = std::list<ScAddr>;

struct RealAddrLessFunc
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

struct ScAddrLessFunc
{
  bool operator()(ScAddr const & a, ScAddr const & b) const
  {
    return RealAddrLessFunc()(*a, *b);
  }
};

// hash functions
template <typename HashType>
struct ScAddrHashFunc
{
  HashType operator()(ScAddr const & addr);
};

template <>
struct ScAddrHashFunc<uint32_t>
{
  uint32_t operator()(ScAddr const & addr) const
  {
    return SC_ADDR_LOCAL_TO_INT(*addr);
  }
};

template <>
struct ScAddrHashFunc<uint64_t>
{
  uint64_t operator()(ScAddr const & addr) const
  {
    return addr.Hash();
  }
};
