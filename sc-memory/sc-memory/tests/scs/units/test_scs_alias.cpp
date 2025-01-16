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

  EXPECT_EQ(parser.GetParsedElement(t.m_connector).GetType(), ScType::ConstActualTempPosArc);
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
  EXPECT_EQ(triples.size(), 2u);

  {
    auto const t = triples.front();
    auto source = parser.GetParsedElement(t.m_source);
    auto connector = parser.GetParsedElement(t.m_connector);
    auto target = parser.GetParsedElement(t.m_target);

    EXPECT_EQ(source.GetIdtf(), "sc_node_tuple");
    EXPECT_TRUE(source.GetType().IsNode());
    EXPECT_TRUE(source.GetType().IsConst());

    EXPECT_EQ(connector.GetType(), ScType::ConstPermPosArc);

    EXPECT_EQ(target.GetIdtf(), "x");
    EXPECT_EQ(target.GetType(), ScType::ConstNodeTuple);
  }

  {
    auto const t = triples.back();
    auto source = parser.GetParsedElement(t.m_source);
    auto connector = parser.GetParsedElement(t.m_connector);
    auto target = parser.GetParsedElement(t.m_target);

    EXPECT_EQ(source.GetIdtf(), "_y");
    EXPECT_TRUE(source.GetType().IsNode());
    EXPECT_TRUE(source.GetType().IsVar());

    EXPECT_EQ(connector.GetType(), ScType::ConstPermNegArc);

    EXPECT_EQ(target.GetIdtf(), "x");
    EXPECT_EQ(target.GetType(), ScType::ConstNodeTuple);
  }
}

TEST(scs_alias, reassign)
{
  std::string const data = "@alias = _x;; _x <- sc_node_struct;; y _/> @alias;; @alias = _[];; z _~> @alias;;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 3u);

  {
    auto const & t = triples[0];

    auto const & source = parser.GetParsedElement(t.m_source);
    auto const & connector = parser.GetParsedElement(t.m_connector);
    auto const & target = parser.GetParsedElement(t.m_target);

    EXPECT_EQ(source.GetIdtf(), "sc_node_struct");
    EXPECT_EQ(source.GetType(), ScType::ConstNode);

    EXPECT_EQ(connector.GetType(), ScType::ConstPermPosArc);

    EXPECT_EQ(target.GetIdtf(), "_x");
    EXPECT_EQ(target.GetType(), ScType::VarNodeStructure);
  }

  {
    auto const & t = triples[1];

    auto const & source = parser.GetParsedElement(t.m_source);
    auto const & connector = parser.GetParsedElement(t.m_connector);
    auto const & target = parser.GetParsedElement(t.m_target);

    EXPECT_EQ(source.GetIdtf(), "y");
    EXPECT_EQ(source.GetType(), ScType::ConstNode);

    EXPECT_EQ(connector.GetType(), ScType::VarFuzArc);

    EXPECT_EQ(target.GetIdtf(), "_x");
    EXPECT_EQ(target.GetType(), ScType::VarNodeStructure);
  }

  {
    auto const & t = triples[2];

    auto const & source = parser.GetParsedElement(t.m_source);
    auto const & connector = parser.GetParsedElement(t.m_connector);
    auto const & target = parser.GetParsedElement(t.m_target);

    EXPECT_EQ(source.GetIdtf(), "z");
    EXPECT_EQ(source.GetType(), ScType::ConstNode);

    EXPECT_EQ(connector.GetType(), ScType::VarActualTempPosArc);

    EXPECT_EQ(target.GetType(), ScType::VarNodeLink);
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

    EXPECT_EQ(src.GetType(), ScType::VarNodeLink);
    EXPECT_EQ(connector.GetType(), ScType::VarActualTempPosArc);
    EXPECT_EQ(trg.GetIdtf(), "y");
  }
}
