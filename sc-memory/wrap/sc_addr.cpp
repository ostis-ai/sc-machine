/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_addr.hpp"

namespace sc
{

Addr::Addr()
{
    SC_ADDR_MAKE_EMPTY(mRealAddr);
}

Addr::Addr(sc_addr const & addr)
    : mRealAddr(addr)
{
}

bool Addr::isValid() const
{
    return !SC_ADDR_IS_EMPTY(mRealAddr);
}

void Addr::reset()
{
    SC_ADDR_MAKE_EMPTY(mRealAddr);
}

bool Addr::operator == (Addr const & other) const
{
    return SC_ADDR_IS_EQUAL(mRealAddr, other.mRealAddr);
}

bool Addr::operator != (Addr const & other) const
{
    return SC_ADDR_IS_NOT_EQUAL(mRealAddr, other.mRealAddr);
}

tRealAddr const & Addr::getRealAddr() const
{
    return mRealAddr;
}

}
