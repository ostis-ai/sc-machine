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
class TIteratorBase
{
public:

    inline bool isValid() const
    {
        return mIterator != 0;
    }

    //! Returns false, if there are no more iterator results. It more results exists, then go to next one and returns true
    virtual bool next() const = 0;

    //! Returns sc-addr of specified element in iterator result
    virtual Addr value(sc_uint8 idx) const = 0;

protected:
    IterType * mIterator;
};


template <typename ParamType1, typename ParamType2, typename ParamType3>
class _SC_WRAP_EXTERN TIterator3 : public TIteratorBase<sc_iterator3>
{
friend class MemoryContext;

protected:
    TIterator3(MemoryContext const & context, ParamType1 const & p1, ParamType2 const & p2, ParamType3 const & p3);

public:
    ~TIterator3()
    {
        destroy();
    }

    TIterator3(TIterator3 const & other)
    {

    }

    TIterator3 & operator = (TIterator3 const & other)
    {
        takeOwnership(other);
        return *this;
    }

    void destroy()
    {     
        if (mIterator)
        {
            sc_iterator3_free(mIterator);
            mIterator = 0;
        }
    }

    inline bool next() const
    {
        check_expr(isValid());
        return sc_iterator3_next(mIterator) == SC_TRUE;
    }

    inline Addr value(sc_uint8 idx) const
    {
        check_expr(isValid());
        return Addr(sc_iterator3_value(mIterator, idx));
    }
};

//typedef Iterator3<Addr, sc_type, Addr> Iterator3_f_a_f;
//typedef Iterator3<Addr, sc_type, sc_type> Iterator3_f_a_a;
//typedef Iterator3<sc_type, sc_type, Addr> Iterator3_a_a_f;


// ---------------------------
template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
class _SC_WRAP_EXTERN TIterator5 : public TIteratorBase<sc_iterator5>
{
    friend class MemoryContext;

protected:
    TIterator5(MemoryContext const & context, ParamType1 const & p1, ParamType2 const & p2, ParamType3 const & p3, ParamType4 const & p4, ParamType5 const & p5);

public:
    ~TIterator5()
    {
        destroy();
    }

    void destroy()
    {
        if (mIterator)
        {
            sc_iterator5_free(mIterator);
            mIterator = 0;
        }
    }

    bool next() const
    {
        check_expr(isValid());
        return sc_iterator5_next(mIterator) == SC_TRUE;
    }

    Addr value(sc_uint8 idx) const
    {
        check_expr(isValid());
        return Addr(sc_iterator5_value(mIterator, idx));
    }

};

//typedef Iterator5<Addr, sc_type, sc_type, sc_type, Addr> Iterator5_f_a_a_a_f;
//typedef Iterator5<sc_type, sc_type, Addr, sc_type, Addr> Iterator5_a_a_f_a_f;
//typedef Iterator5<Addr, sc_type, Addr, sc_type, Addr> Iterator5_f_a_f_a_f;
//typedef Iterator5<Addr, sc_type, Addr, sc_type, sc_type> Iterator5_f_a_f_a_a;
//typedef Iterator5<Addr, sc_type, sc_type, sc_type, sc_type> Iterator5_f_a_a_a_a;
//typedef Iterator5<sc_type, sc_type, Addr, sc_type, sc_type> Iterator5_a_a_f_a_a;

typedef TIteratorBase<sc_iterator3> Iterator3Type;
typedef TIteratorBase<sc_iterator5> Iterator5Type;

typedef TSharedPointer< Iterator3Type > Iterator3Ptr;
typedef TSharedPointer< Iterator5Type > Iterator5Ptr;

}
