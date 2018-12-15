/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"

#include "test_scs_utils.hpp"

UNIT_TEST(scs_level_4)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_4");

  SUBTEST_START(simple_1)
  {
    char const * data = "a -> b: c; d;;";

    scs::Parser parser;
    SC_CHECK(parser.Parse(data), ());

    TripleTester tester(parser);
    tester({
             {
               { ScType::NodeConst, "a" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::NodeConst, "c" }
             },
             {
               { ScType::NodeConst, "b" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
             },
             {
               { ScType::NodeConst, "a" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::NodeConst, "d" }
             },
             {
               { ScType::NodeConst, "b" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
             }
           });

    auto const & triples = parser.GetParsedTriples();

    SC_CHECK_EQUAL(triples.size(), 4, ());

    SC_CHECK_EQUAL(triples[1].m_target, triples[0].m_edge, ());
    SC_CHECK_EQUAL(triples[3].m_target, triples[2].m_edge, ());
  }
  SUBTEST_END()

  SUBTEST_START(simple_2)
  {
    char const * data = "a -> b: c; <- d: e: f;;";

    scs::Parser parser;
    SC_CHECK(parser.Parse(data), ());

    TripleTester tester(parser);
    tester({
             {
               { ScType::NodeConst, "a" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::NodeConst, "c" }
             },
             {
               { ScType::NodeConst, "b" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             },
             {
               { ScType::NodeConst, "f" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::NodeConst, "a" }
             },
             {
               { ScType::NodeConst, "d" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             },
             {
               { ScType::NodeConst, "e" },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             }
           });

    auto const & triples = parser.GetParsedTriples();

    SC_CHECK_EQUAL(triples.size(), 5, ());

    SC_CHECK_EQUAL(triples[0].m_edge, triples[1].m_target, ());
    SC_CHECK_EQUAL(triples[2].m_edge, triples[3].m_target, ());
    SC_CHECK_EQUAL(triples[2].m_edge, triples[4].m_target, ());
  }
  SUBTEST_END()
}
