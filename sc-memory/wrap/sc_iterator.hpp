/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_addr.hpp"
#include "sc_utils.hpp"

extern "C"
{
#include "sc_memory_headers.h"
}

namespace sc
{

class MemoryContext;

template <typename IterType>
class IteratorBase
{

public:
    inline bool isValid() const
    {
        return mIterator != 0;
    }

protected:
    IterType * mIterator;
};


template <typename ParamType1, typename ParamType2, typename ParamType3>
class Iterator3 : public IteratorBase<sc_iterator3>
{
public:
    Iterator3(MemoryContext const & context, ParamType1 const & p1, ParamType2 const & p2, ParamType3 const & p3);
    ~Iterator3()
    {
        destroy();
    }

    void destroy()
    {
        if (mIterator)
            sc_iterator3_free(mIterator);
        mIterator = 0;
    }

    inline bool next() const
    {
        check(isValid());
        return sc_iterator3_next(mIterator) == SC_TRUE;
    }

    inline Addr value(sc_uint8 idx) const
    {
        check(isValid());
        return Addr(sc_iterator3_value(mIterator, idx));
    }


private:
    sc_iterator3 * mIterator;
};

typedef Iterator3<Addr, sc_type, Addr> Iterator3_f_a_f;
typedef Iterator3<Addr, sc_type, sc_type> Iterator3_f_a_a;
typedef Iterator3<sc_type, sc_type, Addr> Iterator3_a_a_f;


template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
class Iterator5 : public IteratorBase<sc_iterator5>
{
public:
    Iterator5(MemoryContext const & context, ParamType1 const & p1, ParamType2 const & p2, ParamType3 const & p3, ParamType4 const & p4, ParamType5 const & p5);
    ~Iterator5()
    {
        destroy();
    }

    void destroy()
    {
        if (mIterator)
            sc_iterator5_free(mIterator);
        mIterator = 0;
    }

    inline bool next() const
    {
        check(isValid());
        return sc_iterator5_next(mIterator) == SC_TRUE;
    }

    inline Addr value(sc_uint8 idx) const
    {
        check(isValid());
        return Addr(sc_iterator5_value(mIterator, idx));
    }

private:
    sc_iterator5 * mIterator;
};

typedef Iterator5<Addr, sc_type, sc_type, sc_type, Addr> Iterator5_f_a_a_a_f;
typedef Iterator5<sc_type, sc_type, Addr, sc_type, Addr> Iterator5_a_a_f_a_f;
typedef Iterator5<Addr, sc_type, Addr, sc_type, Addr> Iterator5_f_a_f_a_f;
typedef Iterator5<Addr, sc_type, Addr, sc_type, sc_type> Iterator5_f_a_f_a_a;
typedef Iterator5<Addr, sc_type, sc_type, sc_type, sc_type> Iterator5_f_a_a_a_a;
typedef Iterator5<sc_type, sc_type, Addr, sc_type, sc_type> Iterator5_a_a_f_a_a;

}
