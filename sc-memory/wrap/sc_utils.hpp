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


class ScException : public std::exception
{
public:
	ScException(std::string const & description);
	virtual ~ScException() throw();

    const char* what() const throw();

private:
    std::string mDescription;
};

#define check_expr(x) assert(x)
#define error(str) { throw ScException(str); }

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
		clear();
    }

	TSharedPointer(TSharedPointer const & other)
	{
		mObject = other._GetPtr();
		mRefCount = other._GetRef();
		mRefCount->ref();
	}

	template <typename OtherType>
    TSharedPointer(TSharedPointer<OtherType> const & other)
    {
        mObject = other._GetPtr();
        mRefCount = other._GetRef();
        mRefCount->ref();
    }

    TSharedPointer & operator = (TSharedPointer const & other)
    {
		clear();
        mObject = other._GetPtr();
        mRefCount = other._GetRef();
        mRefCount->ref();

        return *this;
    }

    ObjectType & operator = (ObjectType * object)
    {
		clear();
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

	void clear()
	{
		if (mRefCount->unref() == 0)
		{
			delete mRefCount;
			delete mObject;
		}

		mRefCount = 0;
		mObject = 0;
	}


protected:
    ObjectType * mObject;
    RefCount * mRefCount;
};

#define SHARED_PTR_TYPE(__type) typedef TSharedPointer< __type > __type##Ptr;

struct MemoryBuffer
{
	char * mData;
	unsigned int mSize;

	MemoryBuffer(char * buff, unsigned int size)
		: mData(buff)
		, mSize(size)
	{
	}
};

SHARED_PTR_TYPE(MemoryBuffer)

template<typename T>
T min(T a, T b)
{
	return (a < b) ? a : b;
}

template<typename T>
T max(T a, T b)
{
	return (a > b) ? a : b;
}
