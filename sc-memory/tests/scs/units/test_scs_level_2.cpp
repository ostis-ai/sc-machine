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
             { ScType::ConstNode, "c" },
             { ScType::ConstPermPosArc, "", scs::Visibility::Local },
             { ScType::ConstNode, "b" }
           },
           {
             { ScType::ConstNode, "a" },
             { ScType::ConstPermPosArc, "", scs::Visibility::Local },
             { ScType::ConstPermPosArc, "", scs::Visibility::Local }
           }
         });

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 2u);
  EXPECT_EQ(triples[0].m_connector, triples[1].m_target);
}

TEST(scs_level_2, simple_2)
{
  char const * data = "(a -> b) => c;;";
  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));
  TripleTester tester(parser);
  tester({
           {
             { ScType::ConstNode, "a" },
             { ScType::ConstPermPosArc, "", scs::Visibility::Local },
             { ScType::ConstNode, "b" }
           },
           {
             { ScType::ConstPermPosArc, "", scs::Visibility::Local },
             { ScType::ConstCommonArc, "", scs::Visibility::Local },
             { ScType::ConstNode, "c" }
           }
         });

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 2u);
  EXPECT_EQ(triples[0].m_connector, triples[1].m_source);
}

TEST(scs_level_2, simple_3)
{
  char const * data = "a -> (b -> (c -> d));;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));
  TripleTester tester(parser);
  tester({
      {
          { ScType::ConstNode, "c" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstNode, "d" }
      },
      {
          { ScType::ConstNode, "b" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local }
      },
      {
          { ScType::ConstNode, "a" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local }
      }
  });

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 3u);
  EXPECT_EQ(triples[0].m_connector, triples[1].m_target);
  EXPECT_EQ(triples[1].m_connector, triples[2].m_target);
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
             { ScType::ConstNode, "b" },
             { ScType::ConstPermPosArc, "", scs::Visibility::Local },
             { ScType::ConstNode, "c" }
           },
           {
             { ScType::ConstNode, "a" },
             { ScType::CommonEdge, "", scs::Visibility::Local },
             { ScType::ConstPermPosArc, "", scs::Visibility::Local }
           },
           {
             { ScType::ConstNode, "x" },
             { ScType::ConstPermPosArc, "", scs::Visibility::Local },
             { ScType::ConstNode, "c" }
           },
           {
             { ScType::ConstNode, "b" },
             { ScType::ConstPermPosArc, "", scs::Visibility::Local },
             { ScType::ConstNode, "y" }
           },
           {
             { ScType::ConstPermPosArc, "", scs::Visibility::Local },
             { ScType::ConstPermPosArc, "", scs::Visibility::Local },
             { ScType::ConstPermPosArc, "", scs::Visibility::Local }
           }
         });

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 5u);

  EXPECT_EQ(triples[0].m_connector, triples[1].m_target);
  EXPECT_EQ(triples[2].m_connector, triples[4].m_target);
  EXPECT_EQ(triples[3].m_connector, triples[4].m_source);
}

TEST(scs_level_2, ordered_set)
{
  char const * data =
      "(b -> rrel_1: c) => a: (b -> d) (*"
      "  => a: (b -> e) (*"
      "    => a: (b -> rrel_last: f);;"
      "  *);;"
      "*);;";

  scs::Parser parser;
  EXPECT_TRUE(parser.Parse(data));

  TripleTester tester(parser);
  tester({
      {
          { ScType::ConstNode, "b" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstNode, "c" }
      },
      {
          { ScType::ConstNode, "rrel_1" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local }
      },
      {
          { ScType::ConstNode, "b" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstNode, "d" }
      },
      {
          { ScType::ConstNode, "b" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstNode, "e" }
      },
      {
          { ScType::ConstNode, "b" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstNode, "f" }
      },
      {
          { ScType::ConstNode, "rrel_last" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local }
      },
      {
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstCommonArc, "", scs::Visibility::Local },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local }
      },
      {
          { ScType::ConstNode, "a" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstCommonArc, "", scs::Visibility::Local }
      },
      {
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstCommonArc, "", scs::Visibility::Local },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local }
      },
      {
          { ScType::ConstNode, "a" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstCommonArc, "", scs::Visibility::Local }
      },
      {
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstCommonArc, "", scs::Visibility::Local },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local }
      },
      {
          { ScType::ConstNode, "a" },
          { ScType::ConstPermPosArc, "", scs::Visibility::Local },
          { ScType::ConstCommonArc, "", scs::Visibility::Local }
      },
  });

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 12u);

  EXPECT_EQ(triples[0].m_connector, triples[1].m_target);
  EXPECT_EQ(triples[0].m_connector, triples[10].m_source);
  EXPECT_EQ(triples[2].m_connector, triples[10].m_target);
  EXPECT_EQ(triples[10].m_connector, triples[11].m_target);
  EXPECT_EQ(triples[2].m_connector, triples[8].m_source);
  EXPECT_EQ(triples[3].m_connector, triples[8].m_target);
  EXPECT_EQ(triples[8].m_connector, triples[9].m_target);
  EXPECT_EQ(triples[3].m_connector, triples[6].m_source);
  EXPECT_EQ(triples[4].m_connector, triples[6].m_target);
  EXPECT_EQ(triples[6].m_connector, triples[7].m_target);
  EXPECT_EQ(triples[4].m_connector, triples[5].m_target);
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
