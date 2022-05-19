/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <gtest/gtest.h>

#include "test_scs_utils.hpp"

TEST(scs_alias, assign)
{

  std::string const data = "@alias = [];; x ~> @alias;;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  auto const & t = triples.front();
  EXPECT_TRUE(parser.GetParsedElement(t.m_source).GetType().IsNode());
  EXPECT_TRUE(parser.GetParsedElement(t.m_target).GetType().IsLink());

  EXPECT_EQ(parser.GetParsedElement(t.m_edge).GetType(), ScType::EdgeAccessConstPosTemp);
}

TEST(scs_alias, no_assign)
{
  std::string const data = "x -> @y;;";

  scs::Parser parser;

  EXPECT_FALSE(parser.Parse(data));
}

TEST(scs_alias, recursive_assigns)
{
  std::string const data = "@alias1 = x;; @alias1 <- sc_node_tuple;; @alias2 = @alias1;; _y -|> x;;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  auto const t = triples.front();
  auto const & src = parser.GetParsedElement(t.m_source);
  auto const & edge = parser.GetParsedElement(t.m_edge);
  auto const & trg = parser.GetParsedElement(t.m_target);

  EXPECT_EQ(src.GetIdtf(), "_y");
  EXPECT_TRUE(src.GetType().IsNode());
  EXPECT_TRUE(src.GetType().IsVar());

  EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstNegPerm);

  EXPECT_EQ(trg.GetIdtf(), "x");
  EXPECT_EQ(trg.GetType(), ScType::NodeConstTuple);
}

TEST(scs_alias, reassign)
{
  std::string const data = "@alias = _x;; _x <- sc_node_struct;; y _~/> @alias;; @alias = _[];; z _~> @alias;;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 2u);

  {
    auto const & t = triples[0];

    auto const & src = parser.GetParsedElement(t.m_source);
    auto const & edge = parser.GetParsedElement(t.m_edge);
    auto const & trg = parser.GetParsedElement(t.m_target);

    EXPECT_EQ(src.GetIdtf(), "y");
    EXPECT_EQ(src.GetType(), ScType::NodeConst);

    EXPECT_EQ(edge.GetType(), ScType::EdgeAccessVarFuzTemp);

    EXPECT_EQ(trg.GetIdtf(), "_x");
    EXPECT_EQ(trg.GetType(), ScType::NodeVarStruct);
  }

  {
    auto const & t = triples[1];

    auto const & src = parser.GetParsedElement(t.m_source);
    auto const & edge = parser.GetParsedElement(t.m_edge);
    auto const & trg = parser.GetParsedElement(t.m_target);

    EXPECT_EQ(src.GetIdtf(), "z");
    EXPECT_EQ(src.GetType(), ScType::NodeConst);

    EXPECT_EQ(edge.GetType(), ScType::EdgeAccessVarPosTemp);

    EXPECT_EQ(trg.GetType(), ScType::LinkVar);
  }
}

TEST(scs_alias, contour)
{
  std::string const data = "@alias = _[];; x -> [* @alias2 = y;; @alias _~> @alias2;;*];;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 5u);

  {
    SPLIT_TRIPLE(triples[0]);

    EXPECT_EQ(src.GetType(), ScType::LinkVar);
    EXPECT_EQ(edge.GetType(), ScType::EdgeAccessVarPosTemp);
    EXPECT_EQ(trg.GetIdtf(), "y");
  }
}
