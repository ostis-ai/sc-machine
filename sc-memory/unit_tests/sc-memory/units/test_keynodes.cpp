/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "sc-memory/sc_keynodes.hpp"
#include "sc-memory/utils/sc_keynode_cache.hpp"

TEST_CASE("keynodes", "[test keynodes]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "keynodes");

  SECTION("binary_type")
  {
    SUBTEST_START("binary_type")
    {
      try
      {
        ScAddr keynodes[] =
              {
                    ScKeynodes::kBinaryDouble,
                    ScKeynodes::kBinaryFloat,
                    ScKeynodes::kBinaryInt8,
                    ScKeynodes::kBinaryInt16,
                    ScKeynodes::kBinaryInt32,
                    ScKeynodes::kBinaryInt64,
                    ScKeynodes::kBinaryUInt8,
                    ScKeynodes::kBinaryUInt16,
                    ScKeynodes::kBinaryUInt32,
                    ScKeynodes::kBinaryUInt64,
                    ScKeynodes::kBinaryString
              };

        for (ScAddr a : keynodes)
          REQUIRE(ctx.HelperCheckEdge(ScKeynodes::kBinaryType, a, ScType::EdgeAccessConstPosPerm));
      } catch (...)
      {
        SC_LOG_ERROR("Test \"binary_type\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("keynode_cache", "[test keynodes]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "keynode_cache");

  SECTION("basic")
  {
    SUBTEST_START("basic")
    {
      try
      {
        utils::ScKeynodeCache cache(ctx);

        ScAddr const addr = ctx.CreateNode(ScType::NodeConst);

        REQUIRE(addr.IsValid());
        REQUIRE(ctx.HelperSetSystemIdtf("test_idtf", addr));

        REQUIRE(cache.GetKeynode("test_idtf").IsValid());
        REQUIRE_FALSE(cache.GetKeynode("other").IsValid());
        REQUIRE_FALSE(cache.GetKeynode("any_idtf").IsValid());
      } catch (...)
      {
        SC_LOG_ERROR("Test \"basic\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
