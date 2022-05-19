/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <gtest/gtest.h>

#include "test_scs_utils.hpp"

TEST(scs_level_3, simple_1)
{
  char const * data = "a -> c: _b:: d;;";
  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));
  TripleTester tester(parser);
  tester({
           {
             { ScType::NodeConst, "a" },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::NodeConst, "d" }
           },
           {
             { ScType::NodeConst, "c" },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
           },
           {
             { ScType::NodeVar, "_b" },
             { ScType::EdgeAccessVarPosPerm, scs::Visibility::Local },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
           }
         });

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 3u);

  EXPECT_EQ(triples[1].m_target, triples[0].m_edge);
  EXPECT_EQ(triples[2].m_target, triples[0].m_edge);
}

TEST(scs_level_3, complex_1)
{
  char const * data = "(a <- f: d) -> (c -> b: d);;";

  scs::Parser parser;
  EXPECT_TRUE(parser.Parse(data));

  TripleTester tester(parser);
  tester({
           {
             { ScType::NodeConst, "d" },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::NodeConst, "a" }
           },
           {
             { ScType::NodeConst, "f" },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
           },
           {
             { ScType::NodeConst, "c" },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::NodeConst, "d" }
           },
           {
             { ScType::NodeConst, "b" },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
           },
           {
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
           }
         });

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 5u);

  EXPECT_EQ(triples[1].m_target, triples[0].m_edge);
  EXPECT_EQ(triples[3].m_target, triples[2].m_edge);
  EXPECT_EQ(triples[4].m_source, triples[0].m_edge);
  EXPECT_EQ(triples[4].m_target, triples[2].m_edge);
}

TEST(scs_level_3, complex_2)
{
  char const * data = "a -> c: (d -> g: h);;";

  scs::Parser parser;
  EXPECT_TRUE(parser.Parse(data));

  TripleTester tester(parser);
  tester({
           {
             { ScType::NodeConst, "d" },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::NodeConst, "h"}
           },
           {
             { ScType::NodeConst, "g" },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
           },
           {
             { ScType::NodeConst, "a" },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
           },
           {
             { ScType::NodeConst, "c" },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
             { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
           }
         });

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 4u);

  EXPECT_EQ(triples[0].m_edge, triples[1].m_target);
  EXPECT_EQ(triples[2].m_target, triples[0].m_edge);
  EXPECT_EQ(triples[3].m_target, triples[2].m_edge);
}
