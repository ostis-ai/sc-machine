/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "test_scs_utils.hpp"

TEST_CASE("scs_level_5", "[test scs level 5]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_5");

  SECTION("simple")
  {
    SUBTEST_START("simple")
    {
      try
      {
        std::string const data = "set ~> attr:: item (* -/> subitem;; <= subitem2;; *);;";

        scs::Parser parser;

        REQUIRE(parser.Parse(data));

        TripleTester tester(parser);
        tester({
                     {
                           { ScType::NodeConst, "item" },
                           { ScType::EdgeAccessConstFuzPerm, scs::Visibility::Local },
                           { ScType::NodeConst,              "subitem" }
                     },
                     {
                           { ScType::NodeConst, "subitem2" },
                           { ScType::EdgeDCommonConst,       scs::Visibility::Local },
                           { ScType::NodeConst,              "item" }
                     },
                     {
                           { ScType::NodeConst, "set" },
                           { ScType::EdgeAccessConstPosTemp, scs::Visibility::Local },
                           { ScType::NodeConst,              "item" }
                     },
                     {
                           { ScType::NodeConst, "attr" },
                           { ScType::EdgeAccessVarPosPerm,   scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosTemp, scs::Visibility::Local }
                     }
               });

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 4);

        REQUIRE(triples[3].m_target == triples[2].m_edge);
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
