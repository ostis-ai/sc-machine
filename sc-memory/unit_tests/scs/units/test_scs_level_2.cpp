/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "test_scs_utils.hpp"

TEST_CASE("scs_level_2", "[test scs level 2]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_2");

  SECTION("simple_1")
  {
    SUBTEST_START("simple_1")
    {
      try
      {
        char const * data = "a -> (b <- c);;";

        scs::Parser parser;

        SC_CHECK(parser.Parse(data), ());
        TripleTester tester(parser);
        tester({
                     {
                           { ScType::NodeConst, "c" },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::NodeConst,              "b" }
                     },
                     {
                           { ScType::NodeConst, "a" },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local }
                     }
               });

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 2);
        REQUIRE(triples[0].m_edge == triples[1].m_target);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"simple_1\" failed");
      }
    }
    SUBTEST_END()
  }

  SECTION("simple_2")
  {
    SUBTEST_START("simple_2")
    {
      try
      {
        char const * data = "(a -> b) => c;;";
        scs::Parser parser;

        REQUIRE(parser.Parse(data));
        TripleTester tester(parser);
        tester(
              {
                    {
                          { ScType::NodeConst,              "a" },
                          { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                          { ScType::NodeConst, "b" }
                    },
                    {
                          { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                          { ScType::EdgeDCommonConst,       "", scs::Visibility::Local },
                          { ScType::NodeConst, "c" }
                    }
              });

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 2);
        REQUIRE(triples[0].m_edge == triples[1].m_source);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"simple_2\" failed");
      }
    }
    SUBTEST_END()
  }

  SECTION("complex")
  {
    SUBTEST_START("complex")
    {
      try
      {
        char const * data =
              "a <> (b -> c);;"
              "(c <- x) <- (b -> y);;";

        scs::Parser parser;
        REQUIRE(parser.Parse(data));

        TripleTester tester(parser);
        tester({
                     {
                           { ScType::NodeConst,              "b" },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::NodeConst,              "c" }
                     },
                     {
                           { ScType::NodeConst,              "a" },
                           { ScType::EdgeUCommon,            "", scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local }
                     },
                     {
                           { ScType::NodeConst,              "x" },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::NodeConst,              "c" }
                     },
                     {
                           { ScType::NodeConst,              "b" },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::NodeConst,              "y" }
                     },
                     {
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local }
                     }
               });

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 5);

        REQUIRE(triples[0].m_edge == triples[1].m_target);
        REQUIRE(triples[2].m_edge == triples[4].m_target);
        REQUIRE(triples[3].m_edge == triples[4].m_source);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"complex\" failed");
      }
    }
    SUBTEST_END()
  }

  SECTION("unnamed")
  {
    SUBTEST_START("unnamed")
    {
      try
      {
        char const * data = "a -> ...; -> ...;;";

        scs::Parser parser;
        REQUIRE(parser.Parse(data));

        auto const triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 2);
        REQUIRE(triples[0].m_source == triples[1].m_source);
        REQUIRE_FALSE(triples[0].m_target == triples[1].m_target);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"unnamed\" failed");
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
