/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"

#include "test_scs_utils.hpp"


UNIT_TEST(scs_ElementHandle)
{
  scs::ElementHandle handle_err;
  SC_CHECK_NOT(handle_err.IsValid(), ());
  SC_CHECK_NOT(handle_err.IsLocal(), ());

  scs::ElementHandle handle_ok(1);
  SC_CHECK(handle_ok.IsValid(), ());
  SC_CHECK_NOT(handle_ok.IsLocal(), ());

  scs::ElementHandle handle_local(0, true);
  SC_CHECK(handle_local.IsValid(), ());
  SC_CHECK(handle_local.IsLocal(), ());
}

UNIT_TEST(scs_parser_error)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_parser_error");

  SUBTEST_START(error_1)
  {
    char const * data = "a -> b;;\nc ->";

    scs::Parser parser;

    SC_CHECK_NOT(parser.Parse(data), ());
    SC_LOG_WARNING(parser.GetParseError());
  }
  SUBTEST_END()
}

UNIT_TEST(scs_parser_triple)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_parser_triple");

  SUBTEST_START(triple_1)
  {
    scs::Parser parser;
    char const * data = "a -> b;;";
    SC_CHECK(parser.Parse(data), ());

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());

    SPLIT_TRIPLE(triples[0]);

    SC_CHECK_EQUAL(src.GetType(), ScType::NodeConst, ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosPerm, ());
    SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConst, ());

    SC_CHECK_EQUAL(src.GetIdtf(), "a", ());
    SC_CHECK_EQUAL(trg.GetIdtf(), "b", ());

    SC_CHECK_EQUAL(src.GetVisibility(), scs::Visibility::System, ());
    SC_CHECK_EQUAL(trg.GetVisibility(), scs::Visibility::System, ());
  }
  SUBTEST_END()

  SUBTEST_START(reversed_1)
  {
    scs::Parser parser;
    char const * data = "a <- b;;";
    SC_CHECK(parser.Parse(data), ());

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());

    SPLIT_TRIPLE(triples[0]);

    SC_CHECK_EQUAL(src.GetIdtf(), "b", ());
    SC_CHECK_EQUAL(trg.GetIdtf(), "a", ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosPerm, ());
  }
  SUBTEST_END()

  SUBTEST_START(sentences_1)
  {
    scs::Parser parser;
    char const * data = "a <- b;; r => x;;";
    SC_CHECK(parser.Parse(data), ());

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 2, ());

    {
      auto const & t = triples[0];
      auto const & source = parser.GetParsedElement(t.m_source);
      auto const & target = parser.GetParsedElement(t.m_target);

      SC_CHECK_EQUAL(source.GetIdtf(), "b", ());
      SC_CHECK_EQUAL(target.GetIdtf(), "a", ());
    }

    {
      SPLIT_TRIPLE(triples[1]);

      SC_CHECK_EQUAL(src.GetIdtf(), "r", ());
      SC_CHECK_EQUAL(trg.GetIdtf(), "x", ());
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeDCommonConst, ());
    }
  }
  SUBTEST_END()
}

UNIT_TEST(scs_comments)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_comments");
  scs::Parser parser;

  char const * data =
      "//Level1\n"
      "a -> b;;/* example */\n"
      "c <> d;;";

  SC_CHECK(parser.Parse(data), ());

  auto const & triples = parser.GetParsedTriples();
  SC_CHECK_EQUAL(triples.size(), 2, ());

  {
    SPLIT_TRIPLE(triples[0]);

    SC_CHECK_EQUAL(src.GetIdtf(), "a", ());
    SC_CHECK_EQUAL(trg.GetIdtf(), "b", ());

    SC_CHECK_EQUAL(src.GetType(), ScType::NodeConst, ());
    SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConst, ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosPerm, ());
  }

  {
    SPLIT_TRIPLE(triples[1]);

    SC_CHECK_EQUAL(src.GetIdtf(), "c", ());
    SC_CHECK_EQUAL(trg.GetIdtf(), "d", ());

    SC_CHECK_EQUAL(src.GetType(), ScType::NodeConst, ());
    SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConst, ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeUCommon, ());
  }
}

UNIT_TEST(scs_const_var)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_const_var");

  char const * data = "_a _-> b;;";
  scs::Parser parser;

  SC_CHECK(parser.Parse(data), ());

  auto const & triples = parser.GetParsedTriples();
  SC_CHECK_EQUAL(triples.size(), 1, ());

  {
    SPLIT_TRIPLE(triples[0]);

    SC_CHECK_EQUAL(src.GetType(), ScType::NodeVar, ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessVarPosPerm, ());
    SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConst, ());

    SC_CHECK_EQUAL(src.GetIdtf(), "_a", ());
    SC_CHECK_EQUAL(trg.GetIdtf(), "b", ());
  }
}

UNIT_TEST(scs_types)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_types");

  SUBTEST_START(nodes)
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

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 4, ());
    {
      auto const CheckSourceNode = [&triples, &parser](size_t index, ScType type)
      {
        SC_CHECK(index < triples.size(), ());
        return (parser.GetParsedElement(triples[index].m_source).GetType() == type);
      };

      auto const CheckTargetNode = [&triples, &parser](size_t index, ScType type)
      {
        SC_CHECK(index < triples.size(), ());
        return (parser.GetParsedElement(triples[index].m_target).GetType() == type);
      };

      SC_CHECK(CheckSourceNode(0, ScType::NodeConstTuple), ());
      SC_CHECK(CheckTargetNode(0, ScType::NodeConstStruct), ());
      SC_CHECK(CheckSourceNode(1, ScType::NodeConstRole), ());
      SC_CHECK(CheckTargetNode(1, ScType::NodeVarNoRole), ());
      SC_CHECK(CheckSourceNode(2, ScType::NodeConstClass), ());
      SC_CHECK(CheckTargetNode(2, ScType::NodeConstAbstract), ());
      SC_CHECK(CheckSourceNode(3, ScType::NodeConstAbstract), ());
      SC_CHECK(CheckTargetNode(3, ScType::NodeConstMaterial), ());
    }
  }
  SUBTEST_END()

  SUBTEST_START(links)
  {
    std::string const data =
        "a -> \"file://data.txt\";;"
        "b -> [x];;"
        "c -> _[];;"
        "d -> [];;";
    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();

    SC_CHECK_EQUAL(triples.size(), 4, ());

    SC_CHECK_EQUAL(parser.GetParsedElement(triples[0].m_target).GetType(), ScType::LinkConst, ());
    SC_CHECK(parser.GetParsedElement(triples[0].m_target).IsURL(), ());
    SC_CHECK_EQUAL(parser.GetParsedElement(triples[1].m_target).GetType(), ScType::LinkConst, ());
    SC_CHECK_NOT(parser.GetParsedElement(triples[2].m_target).IsURL(), ());
    SC_CHECK_EQUAL(parser.GetParsedElement(triples[2].m_target).GetType(), ScType::LinkVar, ());
    SC_CHECK_EQUAL(parser.GetParsedElement(triples[3].m_target).GetType(), ScType::LinkConst, ());
    SC_CHECK_NOT(parser.GetParsedElement(triples[3].m_target).IsURL(), ());
  }
  SUBTEST_END()

  SUBTEST_START(backward_compatibility)
  {
    std::string const data = "a <- c;; a <- sc_node_not_relation;; b <- c;; b <- sc_node_not_binary_tuple;;";
    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 2, ());

    SC_CHECK_EQUAL(parser.GetParsedElement(triples[0].m_target).GetType(), ScType::NodeConstClass, ());
    SC_CHECK_EQUAL(parser.GetParsedElement(triples[1].m_target).GetType(), ScType::NodeConstTuple, ());
  }
  SUBTEST_END()

  SUBTEST_START(edges)
  {
    std::string const data = "x"
                             "> _y; <> y4; ..> y5;"
                             "<=> y7; _<=> y8; => y9; _=> y11;"
                             "-> y2; _-> y13; -|> y15; _-|> y17; -/> y19; _-/> y21;"
                             " ~> y23; _~> y25; ~|> y27; _~|> y29; ~/> y31; _~/> y33;;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 19, ());
    {
      auto const CheckEdgeType = [&triples, &parser](size_t index, ScType type) -> bool
      {
        SC_CHECK(index < triples.size(), ("Invalid index, check test logic please"));
        return (parser.GetParsedElement(triples[index].m_edge).GetType() == type);
      };

      SC_CHECK(CheckEdgeType(0, ScType::EdgeDCommon), ());
      SC_CHECK(CheckEdgeType(1, ScType::EdgeUCommon), ());
      SC_CHECK(CheckEdgeType(2, ScType::EdgeAccess), ());

      SC_CHECK(CheckEdgeType(3, ScType::EdgeUCommonConst), ());
      SC_CHECK(CheckEdgeType(4, ScType::EdgeUCommonVar), ());
      SC_CHECK(CheckEdgeType(5, ScType::EdgeDCommonConst), ());
      SC_CHECK(CheckEdgeType(6, ScType::EdgeDCommonVar), ());

      SC_CHECK(CheckEdgeType(7, ScType::EdgeAccessConstPosPerm), ());
      SC_CHECK(CheckEdgeType(8, ScType::EdgeAccessVarPosPerm), ());
      SC_CHECK(CheckEdgeType(9, ScType::EdgeAccessConstNegPerm), ());
      SC_CHECK(CheckEdgeType(10, ScType::EdgeAccessVarNegPerm), ());
      SC_CHECK(CheckEdgeType(11, ScType::EdgeAccessConstFuzPerm), ());
      SC_CHECK(CheckEdgeType(12, ScType::EdgeAccessVarFuzPerm), ());

      SC_CHECK(CheckEdgeType(13, ScType::EdgeAccessConstPosTemp), ());
      SC_CHECK(CheckEdgeType(14, ScType::EdgeAccessVarPosTemp), ());
      SC_CHECK(CheckEdgeType(15, ScType::EdgeAccessConstNegTemp), ());
      SC_CHECK(CheckEdgeType(16, ScType::EdgeAccessVarNegTemp), ());
      SC_CHECK(CheckEdgeType(17, ScType::EdgeAccessConstFuzTemp), ());
      SC_CHECK(CheckEdgeType(18, ScType::EdgeAccessVarFuzTemp), ());
    }
  }
  SUBTEST_END()

  SUBTEST_START(type_error)
  {
    std::string const data = "a <- sc_node_abstract;; a <- sc_node_role_relation;;";

    scs::Parser parser;
    SC_CHECK_NOT(parser.Parse(data), ());
  }
  SUBTEST_END()
}

UNIT_TEST(scs_aliases)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_aliases");

  SUBTEST_START(simple_assign)
  {
    std::string const data = "@alias = [];; x ~> @alias;;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());

    auto const & t = triples.front();
    SC_CHECK(parser.GetParsedElement(t.m_source).GetType().IsNode(), ());
    SC_CHECK_EQUAL(parser.GetParsedElement(t.m_edge).GetType(), ScType::EdgeAccessConstPosTemp, ());
    SC_CHECK(parser.GetParsedElement(t.m_target).GetType().IsLink(), ());
  }
  SUBTEST_END()

  SUBTEST_START(no_assign)
  {
    std::string const data = "x -> @y;;";

    scs::Parser parser;

    SC_CHECK(!parser.Parse(data), (parser.GetParseError()));
  }
  SUBTEST_END()

  SUBTEST_START(recursive_assigns)
  {
    std::string const data = "@alias1 = x;; @alias1 <- sc_node_tuple;; @alias2 = @alias1;; _y -|> x;;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());

    auto const t = triples.front();
    auto const & src = parser.GetParsedElement(t.m_source);
    auto const & edge = parser.GetParsedElement(t.m_edge);
    auto const & trg = parser.GetParsedElement(t.m_target);

    SC_CHECK_EQUAL(src.GetIdtf(), "_y", ());
    SC_CHECK(src.GetType().IsNode(), ());
    SC_CHECK(src.GetType().IsVar(), ());

    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstNegPerm, ());

    SC_CHECK_EQUAL(trg.GetIdtf(), "x", ());
    SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConstTuple, ());
  }
  SUBTEST_END()

  SUBTEST_START(alias_reassign)
  {
    std::string const data = "@alias = _x;; _x <- sc_node_struct;; y _~/> @alias;; @alias = _[];; z _~> @alias;;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 2, ());

    {
      auto const & t = triples[0];

      auto const & src = parser.GetParsedElement(t.m_source);
      auto const & edge = parser.GetParsedElement(t.m_edge);
      auto const & trg = parser.GetParsedElement(t.m_target);

      SC_CHECK_EQUAL(src.GetIdtf(), "y", ());
      SC_CHECK_EQUAL(src.GetType(), ScType::NodeConst, ());

      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessVarFuzTemp, ());

      SC_CHECK_EQUAL(trg.GetIdtf(), "_x", ());
      SC_CHECK_EQUAL(trg.GetType(), ScType::NodeVarStruct, ());
    }

    {
      auto const & t = triples[1];

      auto const & src = parser.GetParsedElement(t.m_source);
      auto const & edge = parser.GetParsedElement(t.m_edge);
      auto const & trg = parser.GetParsedElement(t.m_target);

      SC_CHECK_EQUAL(src.GetIdtf(), "z", ());
      SC_CHECK_EQUAL(src.GetType(), ScType::NodeConst, ());

      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessVarPosTemp, ());

      SC_CHECK_EQUAL(trg.GetType(), ScType::LinkVar, ());
    }
  }
  SUBTEST_END()
}

UNIT_TEST(scs_file_url)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_file_url");

  std::string const data = "scs_file_url -> \"file://html/faq.html\";;";

  scs::Parser parser;

  SC_CHECK(parser.Parse(data), (parser.GetParseError()));

  auto const & triples = parser.GetParsedTriples();
  SC_CHECK_EQUAL(triples.size(), 1, ());

  SPLIT_TRIPLE(triples[0]);

  SC_CHECK_EQUAL(trg.GetType(), ScType::LinkConst, ());
  SC_CHECK_EQUAL(trg.GetValue(), "file://html/faq.html", ());
  SC_CHECK(trg.IsURL(), ());

}
