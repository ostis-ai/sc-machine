/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"

UNIT_TEST(elements)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "elements");

  ScAddr addr = ctx.CreateNode(sc_type_const);
  SC_CHECK(addr.IsValid(), ());

  ScAddr link = ctx.CreateLink();
  SC_CHECK(link.IsValid(), ());

  ScAddr arc = ctx.CreateEdge(sc_type_arc_pos_const_perm, addr, link);
  SC_CHECK(arc.IsValid(), ());

  SC_CHECK(ctx.IsElement(addr), ());
  SC_CHECK(ctx.IsElement(link), ());
  SC_CHECK(ctx.IsElement(arc), ());

  SC_CHECK_EQUAL(ctx.GetEdgeSource(arc), addr, ());
  SC_CHECK_EQUAL(ctx.GetEdgeTarget(arc), link, ());

  SC_CHECK_EQUAL(ctx.GetElementType(addr), ScType(sc_type_node | sc_type_const), ());
  SC_CHECK_EQUAL(ctx.GetElementType(link), ScType(sc_type_link), ());
  SC_CHECK_EQUAL(ctx.GetElementType(arc), ScType(sc_type_arc_pos_const_perm), ());

  SC_CHECK(ctx.SetElementSubtype(addr, sc_type_var), ());
  SC_CHECK_EQUAL(ctx.GetElementType(addr), ScType(sc_type_node | sc_type_var), ());

  SC_CHECK(ctx.EraseElement(addr), ());
  SC_CHECK(!ctx.IsElement(addr), ());
  SC_CHECK(!ctx.IsElement(arc), ());
  SC_CHECK(ctx.IsElement(link), ());
}

