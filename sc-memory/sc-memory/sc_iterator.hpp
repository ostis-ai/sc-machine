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
#include "sc-core/sc_memory_headers.h"
}

class ScMemoryContext;

template <typename IterType>
class TIteratorBase
{
public:
  virtual ~TIteratorBase()
  {
  }

  inline bool IsValid() const
  {
    return m_iterator != 0;
  }

  //! Returns false, if there are no more iterator results. It more results exists, then go to next one and returns true
  _SC_EXTERN virtual bool Next() const = 0;

  //! Returns sc-addr of specified element in iterator result
  _SC_EXTERN virtual ScAddr Get(sc_uint8 idx) const = 0;

  //! Short form of Get
  inline ScAddr operator[](sc_uint8 idx) const
  {
    return Get(idx);
  }

protected:
  IterType * m_iterator;
};

template <typename ParamType1, typename ParamType2, typename ParamType3>
class TIterator3 : public TIteratorBase<sc_iterator3>
{
  friend class ScMemoryContext;

protected:
  _SC_EXTERN TIterator3(
      ScMemoryContext const & context,
      ParamType1 const & p1,
      ParamType2 const & p2,
      ParamType3 const & p3);

public:
  _SC_EXTERN virtual ~TIterator3()
  {
    Destroy();
  }

  TIterator3(TIterator3 const & other)
  {
  }

  TIterator3 & operator=(TIterator3 const & other)
  {
    TakeOwnership(other);
    return *this;
  }

  void Destroy()
  {
    if (m_iterator)
    {
      sc_iterator3_free(m_iterator);
      m_iterator = 0;
    }
  }

  _SC_EXTERN bool Next() const
  {
    SC_ASSERT(IsValid(), ());
    return sc_iterator3_next(m_iterator) == SC_TRUE;
  }

  _SC_EXTERN ScAddr Get(sc_uint8 idx) const
  {
    SC_ASSERT(idx < 3, ());
    SC_ASSERT(IsValid(), ());
    return ScAddr(sc_iterator3_value(m_iterator, idx));
  }
};

// ---------------------------
template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
class TIterator5 : public TIteratorBase<sc_iterator5>
{
  friend class ScMemoryContext;

protected:
  _SC_EXTERN TIterator5(
      ScMemoryContext const & context,
      ParamType1 const & p1,
      ParamType2 const & p2,
      ParamType3 const & p3,
      ParamType4 const & p4,
      ParamType5 const & p5);

public:
  _SC_EXTERN virtual ~TIterator5()
  {
    Destroy();
  }

  void Destroy()
  {
    if (m_iterator)
    {
      sc_iterator5_free(m_iterator);
      m_iterator = 0;
    }
  }

  _SC_EXTERN bool Next() const
  {
    SC_ASSERT(IsValid(), ());
    return sc_iterator5_next(m_iterator) == SC_TRUE;
  }

  _SC_EXTERN ScAddr Get(sc_uint8 idx) const
  {
    SC_ASSERT(idx < 5, ());
    SC_ASSERT(IsValid(), ());
    return ScAddr(sc_iterator5_value(m_iterator, idx));
  }
};

typedef TIteratorBase<sc_iterator3> ScIterator3Type;
typedef TIteratorBase<sc_iterator5> ScIterator5Type;

typedef std::shared_ptr<ScIterator3Type> ScIterator3Ptr;
typedef std::shared_ptr<ScIterator5Type> ScIterator5Ptr;
