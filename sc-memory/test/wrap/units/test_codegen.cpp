/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"

#include "sc-memory/cpp/utils/sc_test.hpp"

#include "test_sc_object.hpp"

TEST_CASE("Codegen keynodes", "[test codegen]")
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "codegen_keynodes");

  ScAddr addr1 = ctx.CreateNode(ScType::Const);
  REQUIRE(addr1.IsValid());
  REQUIRE(ctx.HelperSetSystemIdtf("test_keynode1", addr1));

  ScAddr addr2 = ctx.CreateNode(ScType::Var);
  REQUIRE(addr2.IsValid());
  REQUIRE(ctx.HelperSetSystemIdtf("test_keynode2", addr2));

  ScAddr addr3 = ctx.CreateNode(ScType::Var);
  REQUIRE(addr3.IsValid());
  REQUIRE(ctx.HelperSetSystemIdtf("test_keynode3", addr3));

  n1::n2::TestObject obj1;
  obj1.Init();

  SC_CHECK_EQUAL(addr1, obj1.mTestKeynode1, ());
  SC_CHECK_EQUAL(addr2, obj1.mTestKeynode2, ());

  obj1.InitGlobal();
  SC_CHECK_EQUAL(addr3, obj1.mTestKeynode3, ());

  ScAddr const addrForce = ctx.HelperFindBySystemIdtf("test_keynode_force");
  REQUIRE(addrForce.IsValid());
  SC_CHECK_EQUAL(addrForce, obj1.mTestKeynodeForce, ());

  ctx.Destroy();
}
