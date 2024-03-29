/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiPrecompiled.h"
#include "uiTypes.h"

bool operator<(sc_addr const & addr1, sc_addr const & addr2)
{
  if (addr1.seg != addr2.seg)
    return addr1.seg < addr2.seg;

  return addr1.offset < addr2.offset;
}

bool operator==(sc_addr const & addr1, sc_addr const & addr2)
{
  return (addr1.seg == addr2.seg) && (addr1.offset == addr2.offset);
}

bool operator!=(sc_addr const & addr1, sc_addr const & addr2)
{
  return (addr1.seg != addr2.seg) || (addr1.offset != addr2.offset);
}
