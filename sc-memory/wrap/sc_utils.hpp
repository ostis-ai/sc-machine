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
	_SC_EXTERN ScException(std::string const & description);
	_SC_EXTERN virtual ~ScException() throw();

	_SC_EXTERN const char* what() const throw();

private:
    std::string mDescription;
};

class ScExceptionInvalidParams final : public ScException
{
public:
	_SC_EXTERN ScExceptionInvalidParams(std::string const & description)
		: ScException(description)
	{
	}
};

#define check_expr(__x) assert(__x)
#define error(__str) { throw ScException(__str); }
#define error_invalid_params(__str) { throw ScExceptionInvalidParams(__str); }

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
        return *mObject;
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

protected:
	MemoryBuffer()
		: mData(0)
		, mSize(0)
	{
	}

};

struct MemoryBufferSafe : public MemoryBuffer
{
	MemoryBufferSafe(char const * buff, unsigned int size)
		: MemoryBuffer(0, size)
	{
		mData = new char[size];
		mSize = size;
	}

	~MemoryBufferSafe()
	{
		delete mData;
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

namespace utils
{
	class StringUtils
	{
	public:
		_SC_EXTERN static std::string replaceAll(std::string const & source, std::string const & replaceWhat, std::string const & replaceWithWhat);
	};
}