/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "../test.hpp"
#include "sc-memory/cpp/sc_struct.hpp"

UNIT_TEST(struct_common)
{
	ScMemoryContext ctx(sc_access_lvl_make_min);

	ScAddr structAddr = ctx.createNode(sc_type_node_struct | sc_type_const);
	SC_CHECK(structAddr.isValid(), ());

	ScStruct st(&ctx, structAddr);

	ScAddr const addr1 = ctx.createNode(sc_type_node_class);
	SC_CHECK(addr1.isValid(), ());

	ScAddr const addr2 = ctx.createNode(sc_type_node_material);
	SC_CHECK(addr2.isValid(), ());

	st << addr1 << addr2;
	SC_CHECK(st.hasElement(addr1), ());
	SC_CHECK(st.hasElement(addr2), ());

	st >> addr1;

	SC_CHECK(!st.hasElement(addr1), ());
	SC_CHECK(st.hasElement(addr2), ());

	st >> addr2;

	SC_CHECK(!st.hasElement(addr1), ());
	SC_CHECK(!st.hasElement(addr2), ());
	SC_CHECK(st.isEmpty(), ());

	// attributes
	ScAddr const attrAddr = ctx.createNode(sc_type_node_role);
	SC_CHECK(attrAddr.isValid(), ());

	SC_CHECK(st.append(addr1, attrAddr), ());
	ScIterator5Ptr iter5 = ctx.iterator5(
		structAddr,
		SC_TYPE(sc_type_arc_pos_const_perm),
		SC_TYPE(0),
		SC_TYPE(sc_type_arc_pos_const_perm),
		attrAddr);

	bool found = false;
	while (iter5->next())
	{
		SC_CHECK(!found, ());	// one time
		SC_CHECK_EQUAL(iter5->value(0), structAddr, ());
		SC_CHECK_EQUAL(iter5->value(2), addr1, ());
		SC_CHECK_EQUAL(iter5->value(4), attrAddr, ());
		found = true;
	}
	SC_CHECK(found, ());
}
