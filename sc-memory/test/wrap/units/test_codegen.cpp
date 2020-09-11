/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <exception>

#include "catch2/catch.hpp"
#include "sc-memory/cpp/utils/sc_test.hpp"
#include "test_sc_object.hpp"

TEST_CASE("Codegen keynodes", "[test codegen]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "codegen_keynodes");

  std::exception_ptr ptr;

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

    SC_CHECK_EQUAL(addr1, testObject.mTestKeynode1, ());
    SC_CHECK_EQUAL(addr2, testObject.mTestKeynode2, ());

    testObject.InitGlobal();
    SC_CHECK_EQUAL(addr3, testObject.mTestKeynode3, ());

    ScAddr const addrForce = ctx.HelperFindBySystemIdtf("test_keynode_force");
    REQUIRE(addrForce.IsValid());
    SC_CHECK_EQUAL(addrForce, testObject.mTestKeynodeForce, ());
  } catch (...)
  {
    SC_LOG_ERROR("Test \"Codegen keynodes\" failed");
  }

  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}
