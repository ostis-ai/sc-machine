/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/sc_keynodes.hpp"

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
