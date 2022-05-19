/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <gtest/gtest.h>

#include "test_scs_utils.hpp"

TEST(scs_level_2, simple_1)
{
  char const * data = "a -> (b <- c);;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));
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
  EXPECT_EQ(triples.size(), 2u);
  EXPECT_EQ(triples[0].m_edge, triples[1].m_target);
}

TEST(scs_level_2, simple_2)
{
  char const * data = "(a -> b) => c;;";
  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));
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
  EXPECT_EQ(triples.size(), 2u);
  EXPECT_EQ(triples[0].m_edge, triples[1].m_source);
}

TEST(scs_level_2, complex)
{
  char const * data =
      "a <> (b -> c);;"
      "(c <- x) <- (b -> y);;";

  scs::Parser parser;
  EXPECT_TRUE(parser.Parse(data));

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
  EXPECT_EQ(triples.size(), 5u);

  EXPECT_EQ(triples[0].m_edge, triples[1].m_target);
  EXPECT_EQ(triples[2].m_edge, triples[4].m_target);
  EXPECT_EQ(triples[3].m_edge, triples[4].m_source);
}

TEST(scs_level_2, unnamed)
{
  char const * data = "a -> ...; -> ...;;";

  scs::Parser parser;
  EXPECT_TRUE(parser.Parse(data));

  auto const triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 2u);
  EXPECT_EQ(triples[0].m_source, triples[1].m_source);
  EXPECT_NE(triples[0].m_target, triples[1].m_target);
}
