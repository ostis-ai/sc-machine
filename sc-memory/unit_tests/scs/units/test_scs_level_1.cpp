/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "test_scs_utils.hpp"

TEST_CASE("scs_level_1", "[test scs level 1]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_1");

  SECTION("simple")
  {
    SUBTEST_START("simple")
    {
      try
      {
        char const * data = "sc_node#a | sc_edge#e1 | sc_node#b;;";
        scs::Parser parser;

        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size()==1);
        {
          SPLIT_TRIPLE(triples[0]);

          REQUIRE(src.GetType()==ScType::NodeConst);
          REQUIRE(trg.GetType()==ScType::NodeConst);
          REQUIRE(edge.GetType()==ScType::EdgeUCommonConst);
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"simple\" failed");
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
