/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "../test.hpp"

UNIT_TEST(elements)
{
	ScMemoryContext ctx;
	ScAddr addr = ctx.createNode(sc_type_const);
	SC_CHECK(addr.isValid(), ());

	ScAddr link = ctx.createLink();
	SC_CHECK(link.isValid(), ());

	ScAddr arc = ctx.createEdge(sc_type_arc_pos_const_perm, addr, link);
	SC_CHECK(arc.isValid(), ());

	SC_CHECK(ctx.isElement(addr), ());
	SC_CHECK(ctx.isElement(link), ());
	SC_CHECK(ctx.isElement(arc), ());

	SC_CHECK_EQUAL(ctx.getEdgeSource(arc), addr, ());
	SC_CHECK_EQUAL(ctx.getEdgeTarget(arc), link, ());

	SC_CHECK_EQUAL(ctx.getElementType(addr), ScType(sc_type_node | sc_type_const), ());
	SC_CHECK_EQUAL(ctx.getElementType(link), ScType(sc_type_link), ());
	SC_CHECK_EQUAL(ctx.getElementType(arc), ScType(sc_type_arc_pos_const_perm), ());

	SC_CHECK(ctx.setElementSubtype(addr, sc_type_var), ());
	SC_CHECK_EQUAL(ctx.getElementType(addr), ScType(sc_type_node | sc_type_var), ());

	SC_CHECK(ctx.eraseElement(addr), ());
	SC_CHECK(!ctx.isElement(addr), ());
	SC_CHECK(!ctx.isElement(arc), ());
	SC_CHECK(ctx.isElement(link), ());
}

