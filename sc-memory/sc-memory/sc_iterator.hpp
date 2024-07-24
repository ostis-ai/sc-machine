/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_addr.hpp"
#include "sc_type.hpp"
#include "sc_utils.hpp"

class ScMemoryContext;

template <typename IterType, sc_uint8 tripleSize>
class _SC_EXTERN TIteratorBase
{
public:
  _SC_EXTERN virtual ~TIteratorBase() = default;

  inline _SC_EXTERN bool IsValid() const
  {
    return m_iterator != nullptr;
  }

  //! Returns false, if there are no more iterator results. It more results exists, then go to next one and returns true
  _SC_EXTERN virtual bool Next() const = 0;

  //! Returns sc-addr of specified element in iterator result
  _SC_EXTERN virtual ScAddr Get(size_t index) const = 0;

  //! Returns sc-addr triple
  _SC_EXTERN virtual std::array<ScAddr, tripleSize> Get() const = 0;

  //! Short form of Get
  inline ScAddr operator[](size_t idx) const
  {
    return Get(idx);
  }

protected:
  IterType * m_iterator = nullptr;
  size_t m_tripleSize = tripleSize;

  static sc_type Convert(sc_type const & s);

  static sc_addr Convert(sc_addr const & s);

  static sc_addr Convert(ScAddr const & addr);

  static sc_type Convert(ScType const & type);
};

using ScAddrTriple = std::array<ScAddr, 3>;
using ScAddrQuintuple = std::array<ScAddr, 5>;

template <typename ParamType1, typename ParamType2, typename ParamType3>
class _SC_EXTERN TIterator3 : public TIteratorBase<sc_iterator3, 3>
{
  friend class ScMemoryContext;

protected:
  _SC_EXTERN TIterator3(
      ScMemoryContext const & context,
      ParamType1 const & p1,
      ParamType2 const & p2,
      ParamType3 const & p3);

public:
  _SC_EXTERN virtual ~TIterator3();

  _SC_EXTERN TIterator3(TIterator3 const & other);

  _SC_EXTERN TIterator3 & operator=(TIterator3 const & other);

  _SC_EXTERN void Destroy();

  _SC_EXTERN bool Next() const override;

  _SC_EXTERN ScAddr Get(size_t index) const override;

  _SC_EXTERN ScAddrTriple Get() const override;
};

// ---------------------------

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
class _SC_EXTERN TIterator5 : public TIteratorBase<sc_iterator5, 5>
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
  _SC_EXTERN ~TIterator5() override;

  _SC_EXTERN void Destroy();

  _SC_EXTERN bool Next() const override;

  _SC_EXTERN ScAddr Get(size_t index) const override;

  _SC_EXTERN ScAddrQuintuple Get() const override;
};

#include "sc_iterator.tpp"

typedef TIteratorBase<sc_iterator3, 3> ScIterator3Type;
typedef TIteratorBase<sc_iterator5, 5> ScIterator5Type;

typedef std::shared_ptr<ScIterator3Type> ScIterator3Ptr;
typedef std::shared_ptr<ScIterator5Type> ScIterator5Ptr;
