/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_addr.hpp"

ScAddr const ScAddr::Empty;

ScAddr::ScAddr()
{
  SC_ADDR_MAKE_EMPTY(m_realAddr);
}

ScAddr::ScAddr(sc_addr const & addr)
  : m_realAddr(addr)
{
}

ScAddr::ScAddr(ScAddr::HashType const & hash)
{
  m_realAddr.offset = hash & 0xffff;
  m_realAddr.seg = (hash >> 16) & 0xffff;
}

bool ScAddr::IsValid() const
{
  return !SC_ADDR_IS_EMPTY(m_realAddr);
}

void ScAddr::Reset()
{
  SC_ADDR_MAKE_EMPTY(m_realAddr);
}

ScAddr::HashType ScAddr::Hash() const
{
  return ((m_realAddr.seg << 16) | m_realAddr.offset);
}

bool ScAddr::operator==(ScAddr const & other) const
{
  return SC_ADDR_IS_EQUAL(m_realAddr, other.m_realAddr);
}

bool ScAddr::operator!=(ScAddr const & other) const
{
  return SC_ADDR_IS_NOT_EQUAL(m_realAddr, other.m_realAddr);
}

ScRealAddr const & ScAddr::operator*() const
{
  return m_realAddr;
}

ScRealAddr const & ScAddr::GetRealAddr() const
{
  return m_realAddr;
}
