/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_iterator.hpp"
#include "sc_memory.hpp"

namespace sc
{

template<> Iterator3<Addr, sc_type, Addr>::Iterator3(MemoryContext const & context, Addr const & p1, sc_type const & p2, Addr const & p3)
{
    mIterator = sc_iterator3_f_a_f_new(context.getRealContext(), p1.mRealAddr, p2, p3.mRealAddr);
}

template<> Iterator3<Addr, sc_type, sc_type>::Iterator3(MemoryContext const & context, Addr const & p1, sc_type const & p2, sc_type const & p3)
{
    mIterator = sc_iterator3_f_a_a_new(context.getRealContext(), p1.mRealAddr, p2, p3);
}

template<> Iterator3<sc_type, sc_type, Addr>::Iterator3(MemoryContext const & context, sc_type const & p1, sc_type const & p2, Addr const & p3)
{
    mIterator = sc_iterator3_a_a_f_new(context.getRealContext(), p1, p2, p3.mRealAddr);
}


template<> Iterator5<Addr, sc_type, sc_type, sc_type, Addr>::Iterator5(MemoryContext const & context, Addr const & p1, sc_type const & p2, sc_type const & p3, sc_type const & p4, Addr const & p5)
{
    mIterator = sc_iterator5_f_a_a_a_f_new(context.getRealContext(), p1.mRealAddr, p2, p3, p4, p5.mRealAddr);
}

template<> Iterator5<sc_type, sc_type, Addr, sc_type, Addr>::Iterator5(MemoryContext const & context, sc_type const & p1, sc_type const & p2, Addr const & p3, sc_type const & p4, Addr const & p5)
{
    mIterator = sc_iterator5_a_a_f_a_f_new(context.getRealContext(), p1, p2, p3.mRealAddr, p4, p5.mRealAddr);
}

template<> Iterator5<Addr, sc_type, Addr, sc_type, Addr>::Iterator5(MemoryContext const & context, Addr const & p1, sc_type const & p2, Addr const & p3, sc_type const & p4, Addr const & p5)
{
    mIterator = sc_iterator5_f_a_f_a_f_new(context.getRealContext(), p1.mRealAddr, p2, p3.mRealAddr, p4, p5.mRealAddr);
}

template<> Iterator5<Addr, sc_type, Addr, sc_type, sc_type>::Iterator5(MemoryContext const & context, Addr const & p1, sc_type const & p2, Addr const & p3, sc_type const & p4, sc_type const & p5)
{
    mIterator = sc_iterator5_f_a_f_a_a_new(context.getRealContext(), p1.mRealAddr, p2, p3.mRealAddr, p4, p5);
}

template<> Iterator5<Addr, sc_type, sc_type, sc_type, sc_type>::Iterator5(MemoryContext const & context, Addr const & p1, sc_type const & p2, sc_type const & p3, sc_type const & p4, sc_type const & p5)
{
    mIterator = sc_iterator5_f_a_a_a_a_new(context.getRealContext(), p1.mRealAddr, p2, p3, p4, p5);
}

template<> Iterator5<sc_type, sc_type, Addr, sc_type, sc_type>::Iterator5(MemoryContext const & context, sc_type const & p1, sc_type const & p2, Addr const & p3, sc_type const & p4, sc_type const & p5)
{
    mIterator = sc_iterator5_a_a_f_a_a_new(context.getRealContext(), p1, p2, p3.mRealAddr, p4, p5);
}


}
