/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <exception>
#include <string>
#include <assert.h>

#include "sc_debug.hpp"
#include "sc_types.hpp"


// ---------------- Reference counter -----------
class RefCount
{
public:
  RefCount()
    : m_refCount(0)
  {
  }

  inline void Ref()
  {
    ++m_refCount;
  }

  inline sc_uint32 Unref()
  {
    SC_ASSERT(m_refCount > 0, ());
    --m_refCount;

    return m_refCount;
  }

private:
  sc_uint32 m_refCount;
};

// ------------ Shared pointer -------------
template<typename ObjectType>
class TSharedPointer
{
public:
  TSharedPointer()
  {
    m_object = 0;
    InitRef();
  }

  TSharedPointer(ObjectType * object)
  {
    m_object = object;
    InitRef();
  }

  ~TSharedPointer()
  {
    Clear();
  }

  TSharedPointer(TSharedPointer const & other)
  {
    m_object = other.GetPtr();
    m_refCount = other.GetRef();
    m_refCount->Ref();
  }

  template <typename OtherType>
  TSharedPointer(TSharedPointer<OtherType> const & other)
  {
    m_object = other.GetPtr();
    m_refCount = other.GetRef();
    m_refCount->Ref();
  }

  TSharedPointer & operator = (TSharedPointer const & other)
  {
    Clear();
    m_object = other.GetPtr();
    m_refCount = other.GetRef();
    m_refCount->Ref();

    return *this;
  }

  ObjectType & operator = (ObjectType * object)
  {
    Clear();
    m_object = object;
    InitRef();
    return *m_object;
  }

  ObjectType & operator * () const
  {
    SC_ASSERT(IsPtrValid(), ());
    return *m_object;
  }

  ObjectType * operator -> () const
  {
    SC_ASSERT(IsPtrValid(), ());
    return m_object;
  }

  inline bool IsPtrValid() const
  {
    return m_object != 0;
  }

  /// Just for internal usage
  inline ObjectType * GetPtr() const
  {
    return m_object;
  }

  /// Just for internal usage
  inline RefCount * GetRef() const
  {
    return m_refCount;
  }

private:

  void InitRef()
  {
    m_refCount = new RefCount();
    m_refCount->Ref();
  }

  void Clear()
  {
    if (m_refCount->Unref() == 0)
    {
      delete m_refCount;
      delete m_object;
    }

    m_refCount = 0;
    m_object = 0;
  }


protected:
  ObjectType * m_object;
  RefCount * m_refCount;
};

#define SHARED_PTR_TYPE(__type) typedef TSharedPointer< __type > __type##Ptr;

struct MemoryBuffer
{
  char * m_data;
  size_t m_size;

  MemoryBuffer(char * buff, unsigned int size)
    : m_data(buff)
    , m_size(size)
  {
  }

  inline bool IsValid() const { return m_data != nullptr; }

protected:
  MemoryBuffer()
    : m_data(0)
    , m_size(0)
  {
  }

};

struct MemoryBufferSafe : public MemoryBuffer
{
  MemoryBufferSafe(char const * buff, unsigned int size)
    : MemoryBuffer(0, size)
  {
    m_data = new char[size];
    m_size = size;
  }

  ~MemoryBufferSafe()
  {
    delete m_data;
  }
};

SHARED_PTR_TYPE(MemoryBuffer)


namespace utils
{

class StringUtils
{
public:
  _SC_EXTERN static void ToLowerCase(std::string & str);
  _SC_EXTERN static void ToUpperCase(std::string & str);

  _SC_EXTERN static bool StartsWith(std::string const & str, std::string const & pattern, bool lowerCase);
  _SC_EXTERN static bool EndsWith(std::string const & str, std::string const & pattern, bool lowerCase);

  _SC_EXTERN static void SplitFilename(std::string const & qualifiedName, std::string & outBasename, std::string & outPath);

  _SC_EXTERN static void TrimLeft(std::string & str);
  _SC_EXTERN static void TrimRight(std::string & str);
  _SC_EXTERN static void Trim(std::string & str);

  _SC_EXTERN static std::string GetFileExtension(std::string const & filename);
  _SC_EXTERN static std::string NormalizeFilePath(std::string const & init, bool makeLowerCase);

  _SC_EXTERN static std::string ReplaceAll(std::string const & source, std::string const & replaceWhat, std::string const & replaceWithWhat);
};

class Random
{
public:
  _SC_EXTERN static int Int();
};

} // namespace utils
