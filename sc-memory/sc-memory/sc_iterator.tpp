/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_iterator.hpp"

extern "C"
{
#include "sc-core/sc_memory_headers.h"
}

template <typename P1, typename P2, typename P3>
sc_iterator3 * CreateIterator3(ScMemoryContext const & context, P1 const & p1, P2 const & p2, P3 const & p3);

template <typename P1, typename P2, typename P3, typename P4, typename P5>
sc_iterator5 * CreateIterator5(
    ScMemoryContext const & context,
    P1 const & p1,
    P2 const & p2,
    P3 const & p3,
    P4 const & p4,
    P5 const & p5);

// ---------------------------

template <typename IterType, sc_uint8 tripleSize>
sc_type ScIterator<IterType, tripleSize>::Convert(sc_type const & type)
{
  return type;
}

template <typename IterType, sc_uint8 tripleSize>
sc_addr ScIterator<IterType, tripleSize>::Convert(sc_addr const & addr)
{
  return addr;
}

template <typename IterType, sc_uint8 tripleSize>
sc_addr ScIterator<IterType, tripleSize>::Convert(ScAddr const & addr)
{
  return *addr;
}

template <typename IterType, sc_uint8 tripleSize>
sc_type ScIterator<IterType, tripleSize>::Convert(ScType const & type)
{
  return *type;
}

// ---------------------------

template <typename ParamType1, typename ParamType2, typename ParamType3>
ScIterator3<ParamType1, ParamType2, ParamType3>::ScIterator3(
    ScMemoryContext const & context,
    ParamType1 const & p1,
    ParamType2 const & p2,
    ParamType3 const & p3)
{
  m_iterator = CreateIterator3(context, Convert(p1), Convert(p2), Convert(p3));
}

template <typename ParamType1, typename ParamType2, typename ParamType3>
ScIterator3<ParamType1, ParamType2, ParamType3>::~ScIterator3()
{
  Destroy();
}

template <typename ParamType1, typename ParamType2, typename ParamType3>
ScIterator3<ParamType1, ParamType2, ParamType3>::ScIterator3(ScIterator3 const &)
{
}

template <typename ParamType1, typename ParamType2, typename ParamType3>
ScIterator3<ParamType1, ParamType2, ParamType3> & ScIterator3<ParamType1, ParamType2, ParamType3>::operator=(
    ScIterator3 const & other)
{
  TakeOwnership(other);
  return *this;
}

template <typename ParamType1, typename ParamType2, typename ParamType3>
void ScIterator3<ParamType1, ParamType2, ParamType3>::Destroy()
{
  if (m_iterator)
  {
    sc_iterator3_free(m_iterator);
    m_iterator = nullptr;
  }
}

template <typename ParamType1, typename ParamType2, typename ParamType3>
bool ScIterator3<ParamType1, ParamType2, ParamType3>::Next() const
{
  sc_result result;
  sc_bool status = sc_iterator3_next_ext(m_iterator, &result);

  switch (result)
  {
  case SC_RESULT_NO:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified iterator3 is empty to iterate next");
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Unable to iterate next triple due sc-memory context is not authorized");
  default:
    break;
  }

  return status == SC_TRUE;
}

template <typename ParamType1, typename ParamType2, typename ParamType3>
ScAddr ScIterator3<ParamType1, ParamType2, ParamType3>::Get(size_t index) const
{
  sc_result result;
  sc_addr const addr = sc_iterator3_value_ext(m_iterator, index, &result);

  switch (result)
  {
  case SC_RESULT_NO:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified iterator3 is empty to get element by index");

  case SC_RESULT_ERROR_INVALID_PARAMS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Index=" + std::to_string(index) + " must be < size=" + std::to_string(m_tripleSize));

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get sc-element sc-address by index=" + std::to_string(index)
            + " due sc-memory context has not read permissions");
  default:
    break;
  }

  return addr;
}

template <typename ParamType1, typename ParamType2, typename ParamType3>
ScAddrTriple ScIterator3<ParamType1, ParamType2, ParamType3>::Get() const
{
  return {Get(0), Get(1), Get(2)};
}

// ---------------------------

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>::ScIterator5(
    ScMemoryContext const & context,
    ParamType1 const & p1,
    ParamType2 const & p2,
    ParamType3 const & p3,
    ParamType4 const & p4,
    ParamType5 const & p5)
{
  m_iterator = CreateIterator5(context, Convert(p1), Convert(p2), Convert(p3), Convert(p4), Convert(p5));
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>::~ScIterator5()
{
  Destroy();
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
void ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>::Destroy()
{
  if (m_iterator)
  {
    sc_iterator5_free(m_iterator);
    m_iterator = nullptr;
  }
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
bool ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>::Next() const
{
  sc_result result;
  sc_bool status = sc_iterator5_next_ext(m_iterator, &result);

  switch (result)
  {
  case SC_RESULT_NO:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified iterator5 is empty to iterate next");
  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHENTICATED:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Unable to iterate next triple due sc-memory context is not authorized");
  default:
    break;
  }

  return status == SC_TRUE;
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
ScAddr ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>::Get(size_t index) const
{
  sc_result result;
  sc_addr const addr = sc_iterator5_value_ext(m_iterator, index, &result);

  switch (result)
  {
  case SC_RESULT_NO:
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Specified iterator5 is empty to get element by index");

  case SC_RESULT_ERROR_INVALID_PARAMS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Index=" + std::to_string(index) + " must be < size=" + std::to_string(m_tripleSize));

  case SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS:
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get sc-element sc-address by index=" + std::to_string(index)
            + " due sc-memory context has not read permissions");
  default:
    break;
  }

  return addr;
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
ScAddrQuintuple ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>::Get() const
{
  return {Get(0), Get(1), Get(2), Get(3), Get(4)};
}
