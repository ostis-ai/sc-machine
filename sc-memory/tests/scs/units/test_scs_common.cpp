/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <gtest/gtest.h>

#include "test_scs_utils.hpp"


TEST(scs_common, ElementHandle)
{
  scs::ElementHandle handle_err;
  EXPECT_FALSE(handle_err.IsValid());
  EXPECT_FALSE(handle_err.IsLocal());

  scs::ElementHandle handle_ok(1);
  EXPECT_TRUE(handle_ok.IsValid());
  EXPECT_FALSE(handle_ok.IsLocal());

  scs::ElementHandle handle_local(0, true);
  EXPECT_TRUE(handle_local.IsValid());
  EXPECT_TRUE(handle_local.IsLocal());
}

TEST(scs_common, parser_error)
{
  char const * data = "a -> b;;\nc ->";

  scs::Parser parser;

  EXPECT_FALSE(parser.Parse(data));
  SC_LOG_WARNING(parser.GetParseError());
}

TEST(scs_common, parser_triple_1)
{
  scs::Parser parser;
  char const * data = "a -> b;;";
  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  SPLIT_TRIPLE(triples[0]);

  EXPECT_EQ(src.GetType(), ScType::NodeConst);
  EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstPosPerm);
  EXPECT_EQ(trg.GetType(), ScType::NodeConst);

  EXPECT_EQ(src.GetIdtf(), "a");
  EXPECT_EQ(trg.GetIdtf(), "b");

  EXPECT_EQ(src.GetVisibility(), scs::Visibility::System);
  EXPECT_EQ(trg.GetVisibility(), scs::Visibility::System);
}

TEST(scs_common, parser_reversed_1)
{
  scs::Parser parser;
  char const * data = "a <- b;;";
  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  SPLIT_TRIPLE(triples[0]);

  EXPECT_EQ(src.GetIdtf(), "b");
  EXPECT_EQ(trg.GetIdtf(), "a");
  EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstPosPerm);
}

TEST(scs_common, sentences_1)
{
  scs::Parser parser;
  char const * data = "a <- b;; r => x;;";
  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 2u);

  {
    auto const & t = triples[0];
    auto const & source = parser.GetParsedElement(t.m_source);
    auto const & target = parser.GetParsedElement(t.m_target);

    EXPECT_EQ(source.GetIdtf(), "b");
    EXPECT_EQ(target.GetIdtf(), "a");
  }

  {
    SPLIT_TRIPLE(triples[1]);

    EXPECT_EQ(src.GetIdtf(), "r");
    EXPECT_EQ(trg.GetIdtf(), "x");
    EXPECT_EQ(edge.GetType(), ScType::EdgeDCommonConst);
  }
}

TEST(scs_common, scs_comments)
{
  scs::Parser parser;

  char const * data =
      "//Level1\n"
      "a -> b;;/* example */\n"
      "c <> d;;";

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 2u);

  {
    SPLIT_TRIPLE(triples[0]);

    EXPECT_EQ(src.GetIdtf(), "a");
    EXPECT_EQ(trg.GetIdtf(), "b");

    EXPECT_EQ(src.GetType(), ScType::NodeConst);
    EXPECT_EQ(trg.GetType(), ScType::NodeConst);
    EXPECT_EQ(edge.GetType(), ScType::EdgeAccessConstPosPerm);
  }

  {
    SPLIT_TRIPLE(triples[1]);

    EXPECT_EQ(src.GetIdtf(), "c");
    EXPECT_EQ(trg.GetIdtf(), "d");

    EXPECT_EQ(src.GetType(), ScType::NodeConst);
    EXPECT_EQ(trg.GetType(), ScType::NodeConst);
    EXPECT_EQ(edge.GetType(), ScType::EdgeUCommon);
  }
}

TEST(scs_common, const_var)
{
  char const * data = "_a _-> b;;";
  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  {
    SPLIT_TRIPLE(triples[0]);

    EXPECT_EQ(src.GetType(), ScType::NodeVar);
    EXPECT_EQ(edge.GetType(), ScType::EdgeAccessVarPosPerm);
    EXPECT_EQ(trg.GetType(), ScType::NodeConst);

    EXPECT_EQ(src.GetIdtf(), "_a");
    EXPECT_EQ(trg.GetIdtf(), "b");
  }
}

TEST(scs_common, nodes)
{
  char const * data = "a -> b;;"
                      "sc_node_tuple -> a;;"
                      "sc_node_struct -> b;;"
                      "sc_node_role_relation -> c;;"
                      "c -> _d;;"
                      "sc_node_norole_relation -> _d;;"
                      "sc_node_class -> e;;"
                      "e -> f;;"
                      "sc_node_abstract -> f;;"
                      "f -> g;;"
                      "sc_node_material -> g;;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 4u);
  {
    auto const CheckSourceNode = [&triples, &parser](size_t index, ScType type)
    {
      EXPECT_TRUE(index < triples.size());
      return (parser.GetParsedElement(triples[index].m_source).GetType() == type);
    };

    auto const CheckTargetNode = [&triples, &parser](size_t index, ScType type)
    {
      EXPECT_TRUE(index < triples.size());
      return (parser.GetParsedElement(triples[index].m_target).GetType() == type);
    };

    EXPECT_TRUE(CheckSourceNode(0, ScType::NodeConstTuple));
    EXPECT_TRUE(CheckTargetNode(0, ScType::NodeConstStruct));
    EXPECT_TRUE(CheckSourceNode(1, ScType::NodeConstRole));
    EXPECT_TRUE(CheckTargetNode(1, ScType::NodeVarNoRole));
    EXPECT_TRUE(CheckSourceNode(2, ScType::NodeConstClass));
    EXPECT_TRUE(CheckTargetNode(2, ScType::NodeConstAbstract));
    EXPECT_TRUE(CheckSourceNode(3, ScType::NodeConstAbstract));
    EXPECT_TRUE(CheckTargetNode(3, ScType::NodeConstMaterial));
  }
}


TEST(scs_common, links)
{
  std::string const data =
      "a -> \"file://data.txt\";;"
      "b -> [x];;"
      "c -> _[];;"
      "d -> [];;";
  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();

  EXPECT_EQ(triples.size(), 4u);

  EXPECT_EQ(parser.GetParsedElement(triples[0].m_target).GetType(), ScType::LinkConst);
  EXPECT_TRUE(parser.GetParsedElement(triples[0].m_target).IsURL());
  EXPECT_EQ(parser.GetParsedElement(triples[1].m_target).GetType(), ScType::LinkConst);
  EXPECT_FALSE(parser.GetParsedElement(triples[2].m_target).IsURL());
  EXPECT_EQ(parser.GetParsedElement(triples[2].m_target).GetType(), ScType::LinkVar);
  EXPECT_EQ(parser.GetParsedElement(triples[3].m_target).GetType(), ScType::LinkConst);
  EXPECT_FALSE(parser.GetParsedElement(triples[3].m_target).IsURL());
}


TEST(scs_common, backward_compatibility)
{
  std::string const data = "a <- c;; a <- sc_node_not_relation;; b <- c;; b <- sc_node_not_binary_tuple;;";
  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 2u);

  EXPECT_EQ(parser.GetParsedElement(triples[0].m_target).GetType(), ScType::NodeConstClass);
  EXPECT_EQ(parser.GetParsedElement(triples[1].m_target).GetType(), ScType::NodeConstTuple);
}

TEST(scs_common, edges)
{
  std::string const data = "x"
                           "> _y; <> y4; ..> y5;"
                           "<=> y7; _<=> y8; => y9; _=> y11;"
                           "-> y2; _-> y13; -|> y15; _-|> y17; -/> y19; _-/> y21;"
                           " ~> y23; _~> y25; ~|> y27; _~|> y29; ~/> y31; _~/> y33;;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 19u);
  {
    auto const CheckEdgeType = [&triples, &parser](size_t index, ScType type) -> bool
    {
      EXPECT_TRUE(index < triples.size());
      return (parser.GetParsedElement(triples[index].m_edge).GetType() == type);
    };

    EXPECT_TRUE(CheckEdgeType(0, ScType::EdgeDCommon));
    EXPECT_TRUE(CheckEdgeType(1, ScType::EdgeUCommon));
    EXPECT_TRUE(CheckEdgeType(2, ScType::EdgeAccess));

    EXPECT_TRUE(CheckEdgeType(3, ScType::EdgeUCommonConst));
    EXPECT_TRUE(CheckEdgeType(4, ScType::EdgeUCommonVar));
    EXPECT_TRUE(CheckEdgeType(5, ScType::EdgeDCommonConst));
    EXPECT_TRUE(CheckEdgeType(6, ScType::EdgeDCommonVar));

    EXPECT_TRUE(CheckEdgeType(7, ScType::EdgeAccessConstPosPerm));
    EXPECT_TRUE(CheckEdgeType(8, ScType::EdgeAccessVarPosPerm));
    EXPECT_TRUE(CheckEdgeType(9, ScType::EdgeAccessConstNegPerm));
    EXPECT_TRUE(CheckEdgeType(10, ScType::EdgeAccessVarNegPerm));
    EXPECT_TRUE(CheckEdgeType(11, ScType::EdgeAccessConstFuzPerm));
    EXPECT_TRUE(CheckEdgeType(12, ScType::EdgeAccessVarFuzPerm));

    EXPECT_TRUE(CheckEdgeType(13, ScType::EdgeAccessConstPosTemp));
    EXPECT_TRUE(CheckEdgeType(14, ScType::EdgeAccessVarPosTemp));
    EXPECT_TRUE(CheckEdgeType(15, ScType::EdgeAccessConstNegTemp));
    EXPECT_TRUE(CheckEdgeType(16, ScType::EdgeAccessVarNegTemp));
    EXPECT_TRUE(CheckEdgeType(17, ScType::EdgeAccessConstFuzTemp));
    EXPECT_TRUE(CheckEdgeType(18, ScType::EdgeAccessVarFuzTemp));
  }
}


TEST(scs_common, type_error)
{
  std::string const data = "a <- sc_node_abstract;; a <- sc_node_role_relation;;";

  scs::Parser parser;
  EXPECT_FALSE(parser.Parse(data));
}

TEST(scs_common, file_url)
{
  std::string const data = "scs_file_url -> \"file://html/faq.html\";;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 1u);

  SPLIT_TRIPLE(triples[0]);

  EXPECT_EQ(trg.GetType(), ScType::LinkConst);
  EXPECT_EQ(trg.GetValue(), "file://html/faq.html");
  EXPECT_TRUE(trg.IsURL());
}
