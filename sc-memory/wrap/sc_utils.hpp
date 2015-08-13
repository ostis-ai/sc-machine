/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <exception>
#include <string>
#include <assert.h>

#include "sc_types.hpp"

namespace sc
{

class Exception : public std::exception
{
public:
    Exception(std::string const & description);
    virtual ~Exception() throw();

    const char* what() const throw();

private:
    std::string mDescription;
};

#define check_expr(x) assert(x)
#define error(str) { throw sc::Exception(str); }

// ---------------- Reference counter -----------
class RefCount
{
public:
    RefCount()
        : mRefCount(0)
    {
    }

    void ref()
    {
        ++mRefCount;
    }

    sc_uint32 unref()
    {
        check_expr(mRefCount > 0);
        --mRefCount;

        return mRefCount;
    }

private:
    sc_uint32 mRefCount;
};

// ------------ Shared pointer -------------
template<typename ObjectType>
class TSharedPointer
{
public:
    TSharedPointer()
    {
        mObject = 0;
        initRef();
    }

    TSharedPointer(ObjectType * object)
    {
        mObject = object;
        initRef();
    }

    ~TSharedPointer()
    {
        if (mRefCount->unref() == 0)
        {
            delete mRefCount;
            delete mObject;
        }
    }

    template <typename OtherObjectType>
    TSharedPointer(TSharedPointer<OtherObjectType> const & other)
    {
        mObject = other._GetPtr();
        mRefCount = other._GetRef();
        mRefCount->ref();
    }

    template <typename OtherObjectType>
    ObjectType & operator = (TSharedPointer<OtherObjectType> const & other)
    {
        mObject = other._GetPtr();
        mRefCount = other._GetRef();
        mRefCount->ref();

        return *this;
    }

    ObjectType & operator = (ObjectType * object)
    {
        mObject = object;
        initRef();
        return *this;
    }

    ObjectType & operator * () const
    {
        check_expr(isValid());
        return *mObject;
    }

    ObjectType * operator -> () const
    {
        check_expr(isValid());
        return mObject;
    }

    bool isValid() const
    {
        return mObject != 0;
    }

    /// Just for internal usage
    inline ObjectType * _GetPtr() const
    {
        return mObject;
    }

    /// Just for internal usage
    inline RefCount * _GetRef() const
    {
        return mRefCount;
    }

private:
    void initRef()
    {
        mRefCount = new RefCount();
        mRefCount->ref();
    }


protected:
    ObjectType * mObject;
    RefCount * mRefCount;
};

#define SHARED_PTR_TYPE(__type) typedef TSharedPointer< __type > __type##Ptr;

}


