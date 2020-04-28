/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-test-framework/sc_test_unit.hpp"

#include "sc-memory/sc_keynodes.hpp"
#include "sc-memory/utils/sc_keynode_cache.hpp"

UNIT_TEST(keynodes)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "keynodes");

  SUBTEST_START("binary_type")
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
      SC_CHECK(ctx.HelperCheckEdge(ScKeynodes::kBinaryType, a, ScType::EdgeAccessConstPosPerm), ());
  }
  SUBTEST_END()
}

UNIT_TEST(keynode_cache)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "keynode_cache");

  SUBTEST_START("basic")
  {
    utils::ScKeynodeCache cache(ctx);

    ScAddr const addr = ctx.CreateNode(ScType::NodeConst);

    SC_CHECK(addr.IsValid(), ());
    SC_CHECK(ctx.HelperSetSystemIdtf("test_idtf", addr), ());

    SC_CHECK(cache.GetKeynode("test_idtf").IsValid(), ());
    SC_CHECK_NOT(cache.GetKeynode("other").IsValid(), ());
    SC_CHECK_NOT(cache.GetKeynode("any_idtf").IsValid(), ());
  }
  SUBTEST_END()
}