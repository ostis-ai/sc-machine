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
#include "sc-memory/sc_memory_headers.h"
}


class ScMemoryContext;

template <typename IterType>
class TIteratorBase
{
public:

    virtual ~TIteratorBase() {}

    inline bool isValid() const
    {
        return mIterator != 0;
    }

    //! Returns false, if there are no more iterator results. It more results exists, then go to next one and returns true
    _SC_EXTERN virtual bool next() const = 0;

    //! Returns sc-addr of specified element in iterator result
	_SC_EXTERN virtual ScAddr value(sc_uint8 idx) const = 0;

protected:
    IterType * mIterator;
};


template <typename ParamType1, typename ParamType2, typename ParamType3>
class TIterator3 : public TIteratorBase<sc_iterator3>
{
friend class ScMemoryContext;

protected:
	_SC_EXTERN TIterator3(ScMemoryContext const & context, ParamType1 const & p1, ParamType2 const & p2, ParamType3 const & p3);

public:
    _SC_EXTERN virtual ~TIterator3()
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

    _SC_EXTERN bool next() const
    {
        check_expr(isValid());
        return sc_iterator3_next(mIterator) == SC_TRUE;
    }

	_SC_EXTERN ScAddr value(sc_uint8 idx) const
    {
        check_expr(idx < 3);
        check_expr(isValid());
		return ScAddr(sc_iterator3_value(mIterator, idx));
    }
};

// ---------------------------
template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
class TIterator5 : public TIteratorBase<sc_iterator5>
{
    friend class ScMemoryContext;

protected:
	_SC_EXTERN TIterator5(ScMemoryContext const & context, ParamType1 const & p1, ParamType2 const & p2, ParamType3 const & p3, ParamType4 const & p4, ParamType5 const & p5);

public:
    _SC_EXTERN virtual ~TIterator5()
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

    _SC_EXTERN bool next() const
    {
        check_expr(isValid());
        return sc_iterator5_next(mIterator) == SC_TRUE;
    }

	_SC_EXTERN ScAddr value(sc_uint8 idx) const
    {
        check_expr(idx < 5);
        check_expr(isValid());
		return ScAddr(sc_iterator5_value(mIterator, idx));
    }

};

typedef TIteratorBase<sc_iterator3> ScIterator3Type;
typedef TIteratorBase<sc_iterator5> ScIterator5Type;

typedef TSharedPointer< ScIterator3Type > ScIterator3Ptr;
typedef TSharedPointer< ScIterator5Type > ScIterator5Ptr;

