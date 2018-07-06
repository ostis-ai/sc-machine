/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/scs/scs_parser.hpp"

#include <glib.h>

namespace
{

struct TripleElement
{
  TripleElement(ScType const & type)
    : m_type(type)
    , m_visibility(scs::Visibility::System)
  {
  }

  TripleElement(ScType const & type, std::string const & idtf)
    : m_type(type)
    , m_idtf(idtf)
    , m_visibility(scs::Visibility::System)
  {
  }

  TripleElement(ScType const & type, std::string const & idtf, scs::Visibility const & vis)
    : m_type(type)
    , m_idtf(idtf)
    , m_visibility(vis)
  {
  }

  TripleElement(ScType const & type, scs::Visibility const & vis)
    : m_type(type)
    , m_visibility(vis)
  {
  }

  void Test(scs::ParsedElement const & el) const
  {
    SC_CHECK_EQUAL(m_type, el.GetType(), ());
    if (!m_idtf.empty())
    {
      SC_CHECK_EQUAL(m_idtf, el.GetIdtf(), ());
    }
    SC_CHECK_EQUAL(m_visibility, el.GetVisibility(), ());
  }

  ScType m_type;
  std::string m_idtf;
  scs::Visibility m_visibility;
};

struct TripleResult
{
  void Test(scs::Parser const & parser, scs::ParsedTriple const & triple) const
  {
    m_source.Test(parser.GetParsedElement(triple.m_source));
    m_edge.Test(parser.GetParsedElement(triple.m_edge));
    m_target.Test(parser.GetParsedElement(triple.m_target));
  }

  TripleElement m_source;
  TripleElement m_edge;
  TripleElement m_target;
};

using ResultTriples = std::vector<TripleResult>;

struct TripleTester
{
  explicit TripleTester(scs::Parser const & parser) : m_parser(parser) {}

  void operator() (ResultTriples const & resultTriples)
  {
    auto const & triples = m_parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), resultTriples.size(), ());
    for (size_t i = 0; i < triples.size(); ++i)
      resultTriples[i].Test(m_parser, triples[i]);
  }

private:
  scs::Parser const & m_parser;
};

}

UNIT_TEST(scs_parser_error)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_parser_error");

  SUBTEST_START(error_1)
  {
    char const * data = "a -> b;;\nc ->";

    scs::Parser parser(ctx);
    
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
    scs::Parser parser(ctx);
    char const * data = "a -> b;;";
    SC_CHECK(parser.Parse(data), ());

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());
    auto const & t = triples[0];
    auto const & source = parser.GetParsedElement(t.m_source);
    auto const & edge = parser.GetParsedElement(t.m_edge);
    auto const & target = parser.GetParsedElement(t.m_target);

    SC_CHECK_EQUAL(source.GetType(), ScType::NodeConst, ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosPerm, ());
    SC_CHECK_EQUAL(target.GetType(), ScType::NodeConst, ());

    SC_CHECK_EQUAL(source.GetIdtf(), "a", ());
    SC_CHECK_EQUAL(target.GetIdtf(), "b", ());

    SC_CHECK_EQUAL(source.GetVisibility(), scs::Visibility::System, ());
    SC_CHECK_EQUAL(target.GetVisibility(), scs::Visibility::System, ());
  }
  SUBTEST_END()

  SUBTEST_START(reversed_1)
  {
    scs::Parser parser(ctx);
    char const * data = "a <- b;;";
    SC_CHECK(parser.Parse(data), ());

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());
    auto const & t = triples[0];
    auto const & source = parser.GetParsedElement(t.m_source);
    auto const & edge = parser.GetParsedElement(t.m_edge);
    auto const & target = parser.GetParsedElement(t.m_target);

    SC_CHECK_EQUAL(source.GetIdtf(), "b", ());
    SC_CHECK_EQUAL(target.GetIdtf(), "a", ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosPerm, ());
  }
  SUBTEST_END()


  SUBTEST_START(sentences_1)
  {
    scs::Parser parser(ctx);
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
      auto const & t = triples[1];
      auto const & source = parser.GetParsedElement(t.m_source);
      auto const & edge = parser.GetParsedElement(t.m_edge);
      auto const & target = parser.GetParsedElement(t.m_target);

      SC_CHECK_EQUAL(source.GetIdtf(), "r", ());
      SC_CHECK_EQUAL(target.GetIdtf(), "x", ());

      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeDCommonConst, ());
    }
  }
  SUBTEST_END()
}

UNIT_TEST(scs_comments)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_comments");
  scs::Parser parser(ctx);

  char const * data =
      "//Level1\n"
      "a -> b;;/* example */\n"
      "c <> d;;";

  SC_CHECK(parser.Parse(data), ());
  
  auto const & triples = parser.GetParsedTriples();
  SC_CHECK_EQUAL(triples.size(), 2, ());

  {
    auto const & src = parser.GetParsedElement(triples[0].m_source);
    auto const & edge = parser.GetParsedElement(triples[0].m_edge);
    auto const & trg = parser.GetParsedElement(triples[0].m_target);

    SC_CHECK_EQUAL(src.GetIdtf(), "a", ());
    SC_CHECK_EQUAL(trg.GetIdtf(), "b", ());

    SC_CHECK_EQUAL(src.GetType(), ScType::NodeConst, ());
    SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConst, ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessConstPosPerm, ());
  }

  {
    auto const & src = parser.GetParsedElement(triples[1].m_source);
    auto const & edge = parser.GetParsedElement(triples[1].m_edge);
    auto const & trg = parser.GetParsedElement(triples[1].m_target);

    SC_CHECK_EQUAL(src.GetIdtf(), "c", ());
    SC_CHECK_EQUAL(trg.GetIdtf(), "d", ());

    SC_CHECK_EQUAL(src.GetType(), ScType::NodeConst, ());
    SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConst, ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeUCommon, ());
  }
}

UNIT_TEST(scs_level_1)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_1");

  SUBTEST_START(simple)
  {
    char const * data = "sc_node#a | sc_edge#e1 | sc_node#b;;";
    scs::Parser parser(ctx);

    SC_CHECK(parser.Parse(data), ());

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());
    {
      auto const & src = parser.GetParsedElement(triples[0].m_source);
      auto const & edge = parser.GetParsedElement(triples[0].m_edge);
      auto const & trg = parser.GetParsedElement(triples[0].m_target);

      SC_CHECK_EQUAL(src.GetType(), ScType::NodeConst, ());
      SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConst, ());
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeUCommonConst, ());
    }
  }
  SUBTEST_END()
}

UNIT_TEST(scs_const_var)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_const_var");

  char const * data = "_a _-> b;;";
  scs::Parser parser(ctx);

  SC_CHECK(parser.Parse(data), ());

  auto const & triples = parser.GetParsedTriples();
  SC_CHECK_EQUAL(triples.size(), 1, ());

  {
    auto const & src = parser.GetParsedElement(triples[0].m_source);
    auto const & edge = parser.GetParsedElement(triples[0].m_edge);
    auto const & trg = parser.GetParsedElement(triples[0].m_target);

    SC_CHECK_EQUAL(src.GetType(), ScType::NodeVar, ());
    SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeAccessVarPosPerm, ());
    SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConst, ());

    SC_CHECK_EQUAL(src.GetIdtf(), "_a", ());
    SC_CHECK_EQUAL(trg.GetIdtf(), "b", ());
  }
}

UNIT_TEST(scs_level_2)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_2");

  SUBTEST_START(simple_1)
  {
    char const * data = "a -> (b <- c);;";

    scs::Parser parser(ctx);

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
    scs::Parser parser(ctx);

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
    
    scs::Parser parser(ctx);
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
}

UNIT_TEST(scs_level_3)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_3");

  SUBTEST_START(simple_1)
  {
    char const * data = "a -> c: _b:: d;;";
    scs::Parser parser(ctx);

    SC_CHECK(parser.Parse(data), ());
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
    SC_CHECK_EQUAL(triples.size(), 3, ());

    SC_CHECK_EQUAL(triples[1].m_target, triples[0].m_edge, ());
    SC_CHECK_EQUAL(triples[2].m_target, triples[0].m_edge, ());
  }
  SUBTEST_END()

  SUBTEST_START(complex_1)
  {
    char const * data = "(a <- f: d) -> (c -> b: d);;";

    scs::Parser parser(ctx);
    SC_CHECK(parser.Parse(data), ());

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
    SC_CHECK_EQUAL(triples.size(), 5, ());

    SC_CHECK_EQUAL(triples[1].m_target, triples[0].m_edge, ());
    SC_CHECK_EQUAL(triples[3].m_target, triples[2].m_edge, ());
    SC_CHECK_EQUAL(triples[4].m_source, triples[0].m_edge, ());
    SC_CHECK_EQUAL(triples[4].m_target, triples[2].m_edge, ());
  }
  SUBTEST_END()

  SUBTEST_START(complex_2)
  {
    char const * data = "a -> c: (d -> g: h);;";

    scs::Parser parser(ctx);
    SC_CHECK(parser.Parse(data), ());

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
    SC_CHECK_EQUAL(triples.size(), 4, ());

    SC_CHECK_EQUAL(triples[0].m_edge, triples[1].m_target, ());
    SC_CHECK_EQUAL(triples[2].m_target, triples[0].m_edge, ());
    SC_CHECK_EQUAL(triples[3].m_target, triples[2].m_edge, ());
  }
  SUBTEST_END()
}

UNIT_TEST(scs_level_4)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_4");

  SUBTEST_START(simple_1)
  {
    char const * data = "a -> b: c; d;;";

    scs::Parser parser(ctx);
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

    scs::Parser parser(ctx);
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

    scs::Parser parser(ctx);

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
    char const * data =
      "a -> \"file://data.txt\";;"
      "b -> [x];;"
      "c -> _[];;"
      "d -> [];;";
    scs::Parser parser(ctx);

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    auto const & triples = parser.GetParsedTriples();
    
    SC_CHECK_EQUAL(triples.size(), 4, ());

    SC_CHECK_EQUAL(parser.GetParsedElement(triples[0].m_target).GetType(), ScType::Link, ());
    SC_CHECK_EQUAL(parser.GetParsedElement(triples[1].m_target).GetType(), ScType::LinkConst, ());
    SC_CHECK_EQUAL(parser.GetParsedElement(triples[2].m_target).GetType(), ScType::LinkVar, ());
    SC_CHECK_EQUAL(parser.GetParsedElement(triples[3].m_target).GetType(), ScType::LinkConst, ());
  }
  SUBTEST_END()

  SUBTEST_START(backward_compatibility)
  {
    char const * data = "a <- c;; a <- sc_node_not_relation;; b <- c;; b <- sc_node_not_binary_tuple;;";
    scs::Parser parser(ctx);

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));
    
    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 2, ());

    SC_CHECK_EQUAL(parser.GetParsedElement(triples[0].m_target).GetType(), ScType::NodeConstClass, ());
    SC_CHECK_EQUAL(parser.GetParsedElement(triples[1].m_target).GetType(), ScType::NodeConstTuple, ());
  }
  SUBTEST_END()

  SUBTEST_START(edges)
  {
    char const * data = "x"
      "> _y; <> y4; ..> y5;"
      "<=> y7; _<=> y8; => y9; _=> y11;"
      "-> y2; _-> y13; -|> y15; _-|> y17; -/> y19; _-/> y21;"
      " ~> y23; _~> y25; ~|> y27; _~|> y29; ~/> y31; _~/> y33;;";

    scs::Parser parser(ctx);

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
    char const * data = "a <- sc_node_abstract;; a <- sc_node_role_relation;;";

    scs::Parser parser(ctx);
    SC_CHECK_NOT(parser.Parse(data), ());
  }
  SUBTEST_END()
}