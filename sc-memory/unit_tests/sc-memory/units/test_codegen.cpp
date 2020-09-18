/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "test_sc_object.hpp"

TEST_CASE("Codegen keynodes", "[test codegen]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "codegen_keynodes");

  try
  {
    ScAddr addr1 = ctx.CreateNode(ScType::Const);
    REQUIRE(addr1.IsValid());
    REQUIRE(ctx.HelperSetSystemIdtf("test_keynode1", addr1));

    ScAddr addr2 = ctx.CreateNode(ScType::Var);
    REQUIRE(addr2.IsValid());
    REQUIRE(ctx.HelperSetSystemIdtf("test_keynode2", addr2));

    ScAddr addr3 = ctx.CreateNode(ScType::Var);
    REQUIRE(addr3.IsValid());
    REQUIRE(ctx.HelperSetSystemIdtf("test_keynode3", addr3));

    n1::n2::TestObject testObject;
    testObject.Init();

    REQUIRE(addr1 == testObject.mTestKeynode1);
    REQUIRE(addr2 == testObject.mTestKeynode2);

    testObject.InitGlobal();
    REQUIRE(addr3 == testObject.mTestKeynode3);

    ScAddr const addrForce = ctx.HelperFindBySystemIdtf("test_keynode_force");
    REQUIRE(addrForce.IsValid());
    REQUIRE(addrForce == testObject.mTestKeynodeForce);
  } catch (...)
  {
    SC_LOG_ERROR("Test \"Codegen keynodes\" failed");
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
