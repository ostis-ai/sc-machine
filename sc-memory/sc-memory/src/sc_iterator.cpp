/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_iterator.hpp"
#include "sc-memory/sc_memory.hpp"

template <>
sc_iterator3 * CreateIterator3<sc_addr, sc_type, sc_addr>(
    ScMemoryContext const & context,
    sc_addr const & p1,
    sc_type const & p2,
    sc_addr const & p3)
{
  return sc_iterator3_f_a_f_new(*context, p1, p2, p3);
}

template <>
sc_iterator3 * CreateIterator3<sc_addr, sc_type, sc_type>(
    ScMemoryContext const & context,
    sc_addr const & p1,
    sc_type const & p2,
    sc_type const & p3)
{
  return sc_iterator3_f_a_a_new(*context, p1, p2, p3);
}

template <>
sc_iterator3 * CreateIterator3<sc_addr, sc_addr, sc_type>(
    ScMemoryContext const & context,
    sc_addr const & p1,
    sc_addr const & p2,
    sc_type const & p3)
{
  return sc_iterator3_f_f_a_new(*context, p1, p2, p3);
}

template <>
sc_iterator3 * CreateIterator3<sc_type, sc_type, sc_addr>(
    ScMemoryContext const & context,
    sc_type const & p1,
    sc_type const & p2,
    sc_addr const & p3)
{
  return sc_iterator3_a_a_f_new(*context, p1, p2, p3);
}

template <>
sc_iterator3 * CreateIterator3<sc_type, sc_addr, sc_addr>(
    ScMemoryContext const & context,
    sc_type const & p1,
    sc_addr const & p2,
    sc_addr const & p3)
{
  return sc_iterator3_a_f_f_new(*context, p1, p2, p3);
}

template <>
sc_iterator3 * CreateIterator3<sc_type, sc_addr, sc_type>(
    ScMemoryContext const & context,
    sc_type const & p1,
    sc_addr const & p2,
    sc_type const & p3)
{
  return sc_iterator3_a_f_a_new(*context, p1, p2, p3);
}

template <>
sc_iterator3 * CreateIterator3<sc_addr, sc_addr, sc_addr>(
    ScMemoryContext const & context,
    sc_addr const & p1,
    sc_addr const & p2,
    sc_addr const & p3)
{
  return sc_iterator3_f_f_f_new(*context, p1, p2, p3);
}

template <>
sc_iterator5 * CreateIterator5<sc_addr, sc_type, sc_type, sc_type, sc_type>(
    ScMemoryContext const & context,
    sc_addr const & p1,
    sc_type const & p2,
    sc_type const & p3,
    sc_type const & p4,
    sc_type const & p5)
{
  return sc_iterator5_f_a_a_a_a_new(*context, p1, p2, p3, p4, p5);
}

template <>
sc_iterator5 * CreateIterator5<sc_addr, sc_type, sc_addr, sc_type, sc_type>(
    ScMemoryContext const & context,
    sc_addr const & p1,
    sc_type const & p2,
    sc_addr const & p3,
    sc_type const & p4,
    sc_type const & p5)
{
  return sc_iterator5_f_a_f_a_a_new(*context, p1, p2, p3, p4, p5);
}

template <>
sc_iterator5 * CreateIterator5<sc_addr, sc_type, sc_addr, sc_type, sc_addr>(
    ScMemoryContext const & context,
    sc_addr const & p1,
    sc_type const & p2,
    sc_addr const & p3,
    sc_type const & p4,
    sc_addr const & p5)
{
  return sc_iterator5_f_a_f_a_f_new(*context, p1, p2, p3, p4, p5);
}

template <>
sc_iterator5 * CreateIterator5<sc_addr, sc_type, sc_type, sc_type, sc_addr>(
    ScMemoryContext const & context,
    sc_addr const & p1,
    sc_type const & p2,
    sc_type const & p3,
    sc_type const & p4,
    sc_addr const & p5)
{
  return sc_iterator5_f_a_a_a_f_new(*context, p1, p2, p3, p4, p5);
}

template <>
sc_iterator5 * CreateIterator5<sc_type, sc_type, sc_addr, sc_type, sc_addr>(
    ScMemoryContext const & context,
    sc_type const & p1,
    sc_type const & p2,
    sc_addr const & p3,
    sc_type const & p4,
    sc_addr const & p5)
{
  return sc_iterator5_a_a_f_a_f_new(*context, p1, p2, p3, p4, p5);
}

template <>
sc_iterator5 * CreateIterator5<sc_type, sc_type, sc_addr, sc_type, sc_type>(
    ScMemoryContext const & context,
    sc_type const & p1,
    sc_type const & p2,
    sc_addr const & p3,
    sc_type const & p4,
    sc_type const & p5)
{
  return sc_iterator5_a_a_f_a_a_new(*context, p1, p2, p3, p4, p5);
}

template <>
sc_iterator5 * CreateIterator5<sc_type, sc_type, sc_type, sc_type, sc_addr>(
    ScMemoryContext const & context,
    sc_type const & p1,
    sc_type const & p2,
    sc_type const & p3,
    sc_type const & p4,
    sc_addr const & p5)
{
  return sc_iterator5_a_a_a_a_f_new(*context, p1, p2, p3, p4, p5);
}
