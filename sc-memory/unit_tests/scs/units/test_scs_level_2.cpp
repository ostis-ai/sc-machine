/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/utils/sc_test.hpp"

#include "test_scs_utils.hpp"

UNIT_TEST(scs_level_2)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_2");

  SUBTEST_START(simple_1)
  {
    char const * data = "a -> (b <- c);;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), ());
    TripleTester tester(parser);
    tester({
             {
               { ScType::NodeConst, "c" },
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
               { ScType::NodeConst, "b" }
             },
             {
               { ScType::NodeConst, "a" },
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local }
             }
           });

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 2, ());
    SC_CHECK_EQUAL(triples[0].m_edge, triples[1].m_target, ());
  }
  SUBTEST_END()

  SUBTEST_START(simple_2)
  {
    char const * data = "(a -> b) => c;;";
    scs::Parser parser;

    SC_CHECK(parser.Parse(data), ());
    TripleTester tester(parser);
    tester({
             {
               { ScType::NodeConst, "a" },
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
               { ScType::NodeConst, "b" }
             },
             {
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
               { ScType::EdgeDCommonConst, "", scs::Visibility::Local },
               { ScType::NodeConst, "c" }
             }
           });

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 2, ());
    SC_CHECK_EQUAL(triples[0].m_edge, triples[1].m_source, ());
  }
  SUBTEST_END()

  SUBTEST_START(complex)
  {
    char const * data =
        "a <> (b -> c);;"
        "(c <- x) <- (b -> y);;";

    scs::Parser parser;
    SC_CHECK(parser.Parse(data), ());

    TripleTester tester(parser);
    tester({
             {
               { ScType::NodeConst, "b" },
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
               { ScType::NodeConst, "c" }
             },
             {
               { ScType::NodeConst, "a" },
               { ScType::EdgeUCommon, "", scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local }
             },
             {
               { ScType::NodeConst, "x" },
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
               { ScType::NodeConst, "c" }
             },
             {
               { ScType::NodeConst, "b" },
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
               { ScType::NodeConst, "y" }
             },
             {
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
               { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local }
             }
           });

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 5, ());

    SC_CHECK_EQUAL(triples[0].m_edge, triples[1].m_target, ());
    SC_CHECK_EQUAL(triples[2].m_edge, triples[4].m_target, ());
    SC_CHECK_EQUAL(triples[3].m_edge, triples[4].m_source, ());
  }
  SUBTEST_END()

  SUBTEST_START(unnamed)
  {
    char const * data = "a -> ...; -> ...;;";

    scs::Parser parser;
    SC_CHECK(parser.Parse(data), ());

    auto const triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 2, ());
    SC_CHECK_EQUAL(triples[0].m_source, triples[1].m_source, ());
    SC_CHECK_NOT_EQUAL(triples[0].m_target, triples[1].m_target, ());
  }
  SUBTEST_END()
}
