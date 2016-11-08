/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "../test.hpp"

#include "test_sc_object.hpp"
#include "test_sc_agent.hpp"

UNIT_TEST(codegen_keynodes)
{
	ScMemoryContext ctx(sc_access_lvl_make_min);

	ScAddr addr1 = ctx.createNode(sc_type_const);
	SC_CHECK(addr1.isValid(), ());
	SC_CHECK(ctx.helperSetSystemIdtf("test_keynode1", addr1), ());

	ScAddr addr2 = ctx.createNode(sc_type_var);
	SC_CHECK(addr2.isValid(), ());
	SC_CHECK(ctx.helperSetSystemIdtf("test_keynode2", addr2), ());

	ScAddr addr3 = ctx.createNode(sc_type_var);
	SC_CHECK(addr3.isValid(), ());
	SC_CHECK(ctx.helperSetSystemIdtf("test_keynode3", addr3), ());

	n1::n2::TestObject obj1;
	SC_CHECK_EQUAL(addr1, obj1.mTestKeynode1, ());
	SC_CHECK_EQUAL(addr2, obj1.mTestKeynode2, ());

	obj1.initGlobal();
	SC_CHECK_EQUAL(addr3, obj1.mTestKeynode3, ());

	ScAddr addrForce;
	SC_CHECK(ctx.helperFindBySystemIdtf("test_keynode_force", addrForce), ());
	SC_CHECK_EQUAL(addrForce, obj1.mTestKeynodeForce, ());
}
