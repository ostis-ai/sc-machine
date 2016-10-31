/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_iterator.hpp"
#include "sc_memory.hpp"


template<> TIterator3<ScAddr, sc_type, ScAddr>::TIterator3(ScMemoryContext const & context, ScAddr const & p1, sc_type const & p2, ScAddr const & p3)
{
    mIterator = sc_iterator3_f_a_f_new(context.getRealContext(), *p1, p2, *p3);
}

template<> TIterator3<ScAddr, sc_type, sc_type>::TIterator3(ScMemoryContext const & context, ScAddr const & p1, sc_type const & p2, sc_type const & p3)
{
    mIterator = sc_iterator3_f_a_a_new(context.getRealContext(), *p1, p2, p3);
}

template<> TIterator3<sc_type, sc_type, ScAddr>::TIterator3(ScMemoryContext const & context, sc_type const & p1, sc_type const & p2, ScAddr const & p3)
{
    mIterator = sc_iterator3_a_a_f_new(context.getRealContext(), p1, p2, *p3);
}

template<> TIterator3<sc_type, ScAddr, sc_type>::TIterator3(ScMemoryContext const & context, sc_type const & p1, ScAddr const & p2, sc_type const & p3)
{
	mIterator = sc_iterator3_a_f_a_new(*context, p1, *p2, p3);
}

template<> TIterator5<ScAddr, sc_type, sc_type, sc_type, ScAddr>::TIterator5(ScMemoryContext const & context, ScAddr const & p1, sc_type const & p2, sc_type const & p3, sc_type const & p4, ScAddr const & p5)
{
    mIterator = sc_iterator5_f_a_a_a_f_new(context.getRealContext(), *p1, p2, p3, p4, *p5);
}

template<> TIterator5<sc_type, sc_type, ScAddr, sc_type, ScAddr>::TIterator5(ScMemoryContext const & context, sc_type const & p1, sc_type const & p2, ScAddr const & p3, sc_type const & p4, ScAddr const & p5)
{
    mIterator = sc_iterator5_a_a_f_a_f_new(context.getRealContext(), p1, p2, *p3, p4, *p5);
}

template<> TIterator5<ScAddr, sc_type, ScAddr, sc_type, ScAddr>::TIterator5(ScMemoryContext const & context, ScAddr const & p1, sc_type const & p2, ScAddr const & p3, sc_type const & p4, ScAddr const & p5)
{
    mIterator = sc_iterator5_f_a_f_a_f_new(context.getRealContext(), *p1, p2, *p3, p4, *p5);
}

template<> TIterator5<ScAddr, sc_type, ScAddr, sc_type, sc_type>::TIterator5(ScMemoryContext const & context, ScAddr const & p1, sc_type const & p2, ScAddr const & p3, sc_type const & p4, sc_type const & p5)
{
    mIterator = sc_iterator5_f_a_f_a_a_new(context.getRealContext(), *p1, p2, *p3, p4, p5);
}

template<> TIterator5<ScAddr, sc_type, sc_type, sc_type, sc_type>::TIterator5(ScMemoryContext const & context, ScAddr const & p1, sc_type const & p2, sc_type const & p3, sc_type const & p4, sc_type const & p5)
{
    mIterator = sc_iterator5_f_a_a_a_a_new(context.getRealContext(), *p1, p2, p3, p4, p5);
}

template<> TIterator5<sc_type, sc_type, ScAddr, sc_type, sc_type>::TIterator5(ScMemoryContext const & context, sc_type const & p1, sc_type const & p2, ScAddr const & p3, sc_type const & p4, sc_type const & p5)
{
    mIterator = sc_iterator5_a_a_f_a_a_new(context.getRealContext(), p1, p2, *p3, p4, p5);
}


