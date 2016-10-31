/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_addr.hpp"


ScAddr::ScAddr()
{
	SC_ADDR_MAKE_EMPTY(mRealAddr);
}

ScAddr::ScAddr(sc_addr const & addr)
	: mRealAddr(addr)
{
}

bool ScAddr::isValid() const
{
    return !SC_ADDR_IS_EMPTY(mRealAddr);
}

void ScAddr::reset()
{
    SC_ADDR_MAKE_EMPTY(mRealAddr);
}

bool ScAddr::operator == (ScAddr const & other) const
{
    return SC_ADDR_IS_EQUAL(mRealAddr, other.mRealAddr);
}

bool ScAddr::operator != (ScAddr const & other) const
{
    return SC_ADDR_IS_NOT_EQUAL(mRealAddr, other.mRealAddr);
}

tRealAddr ScAddr::operator * () const
{
	return mRealAddr;
}

tRealAddr const & ScAddr::getRealAddr() const
{
    return mRealAddr;
}

