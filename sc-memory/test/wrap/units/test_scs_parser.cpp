/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"

#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/scs/scs_parser.hpp"

#include <glib.h>

namespace
{

struct TripleElement
{
  TripleElement(ScType const & type)
        : m_type(type), m_visibility(scs::Visibility::System)
  {
  }

  TripleElement(ScType const & type, std::string const & idtf)
        : m_type(type), m_idtf(idtf), m_visibility(scs::Visibility::System)
  {
  }

  TripleElement(ScType const & type, std::string const & idtf, scs::Visibility const & vis)
        : m_type(type), m_idtf(idtf), m_visibility(vis)
  {
  }

  TripleElement(ScType const & type, scs::Visibility const & vis)
        : m_type(type), m_visibility(vis)
  {
  }

  void Test(scs::ParsedElement const & el) const
  {
    REQUIRE(m_type == el.GetType());
    if (!m_idtf.empty())
    {
      REQUIRE_THAT(m_idtf, Catch::Equals(el.GetIdtf()));
    }
    REQUIRE(m_visibility == el.GetVisibility());
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
  explicit TripleTester(scs::Parser const & parser) : m_parser(parser)
  {}

  void operator()(ResultTriples const & resultTriples)
  {
    auto const & triples = m_parser.GetParsedTriples();
    REQUIRE(triples.size() == resultTriples.size());
    for (size_t i = 0; i < triples.size(); ++i)
      resultTriples[i].Test(m_parser, triples[i]);
  }

private:
  scs::Parser const & m_parser;
};

}

TEST_CASE("scs_parser_error", "[test scs parser]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_parser_error");

  SECTION("error_1")
  {
    SUBTEST_START("error_1")
    {
      try
      {
        char const * data = "a -> b;;\nc ->";

        scs::Parser parser(ctx);

        REQUIRE_FALSE(parser.Parse(data));
        SC_LOG_WARNING(parser.GetParseError());
      } catch (...)
      {
        SC_LOG_ERROR("Test \"error_1\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_parser_triple", "[test scs parser]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_parser_triple");

  SECTION("triple_1")
  {
    SUBTEST_START("triple_1")
    {
      try
      {
        scs::Parser parser(ctx);
        char const * data = "a -> b;;";
        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 1);
        auto const & t = triples[0];
        auto const & source = parser.GetParsedElement(t.m_source);
        auto const & edge = parser.GetParsedElement(t.m_edge);
        auto const & target = parser.GetParsedElement(t.m_target);

        REQUIRE(source.GetType() == ScType::NodeConst);
        REQUIRE(edge.GetType() == ScType::EdgeAccessConstPosPerm);
        REQUIRE(target.GetType() == ScType::NodeConst);

        REQUIRE_THAT(source.GetIdtf(), Catch::Equals("a"));
        REQUIRE_THAT(target.GetIdtf(), Catch::Equals("b"));

        REQUIRE(source.GetVisibility() == scs::Visibility::System);
        REQUIRE(target.GetVisibility() == scs::Visibility::System);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"triple_1\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("reversed_1")
  {
    SUBTEST_START("reversed_1")
    {
      try
      {
        scs::Parser parser(ctx);
        char const * data = "a <- b;;";
        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 1);
        auto const & t = triples[0];
        auto const & source = parser.GetParsedElement(t.m_source);
        auto const & edge = parser.GetParsedElement(t.m_edge);
        auto const & target = parser.GetParsedElement(t.m_target);

        REQUIRE_THAT(source.GetIdtf(), Catch::Equals("b"));
        REQUIRE_THAT(target.GetIdtf(), Catch::Equals("a"));
        REQUIRE(edge.GetType() == ScType::EdgeAccessConstPosPerm);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"reversed_1\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("sentences_1")
  {
    SUBTEST_START("sentences_1")
    {
      try
      {
        scs::Parser parser(ctx);
        char const * data = "a <- b;; r => x;;";
        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 2);

        {
          auto const & t = triples[0];
          auto const & source = parser.GetParsedElement(t.m_source);
          auto const & target = parser.GetParsedElement(t.m_target);

          REQUIRE_THAT(source.GetIdtf(), Catch::Equals("b"));
          REQUIRE_THAT(target.GetIdtf(), Catch::Equals("a"));
        }

        {
          auto const & t = triples[1];
          auto const & source = parser.GetParsedElement(t.m_source);
          auto const & edge = parser.GetParsedElement(t.m_edge);
          auto const & target = parser.GetParsedElement(t.m_target);

          REQUIRE_THAT(source.GetIdtf(), Catch::Equals("r"));
          REQUIRE_THAT(target.GetIdtf(), Catch::Equals("x"));

          REQUIRE(edge.GetType() == ScType::EdgeDCommonConst);
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"sentences_1\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_comments", "[test scs parser]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_comments");
  scs::Parser parser(ctx);

  char const * data =
        "//Level1\n"
        "a -> b;;/* example */\n"
        "c <> d;;";

  try
  {
    REQUIRE(parser.Parse(data));

    auto const & triples = parser.GetParsedTriples();
    REQUIRE(triples.size() == 2);

    {
      auto const & src = parser.GetParsedElement(triples[0].m_source);
      auto const & edge = parser.GetParsedElement(triples[0].m_edge);
      auto const & trg = parser.GetParsedElement(triples[0].m_target);

      REQUIRE_THAT(src.GetIdtf(), Catch::Equals("a"));
      REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("b"));

      REQUIRE(src.GetType() == ScType::NodeConst);
      REQUIRE(trg.GetType() == ScType::NodeConst);
      REQUIRE(edge.GetType() == ScType::EdgeAccessConstPosPerm);
    }

    {
      auto const & src = parser.GetParsedElement(triples[1].m_source);
      auto const & edge = parser.GetParsedElement(triples[1].m_edge);
      auto const & trg = parser.GetParsedElement(triples[1].m_target);

      REQUIRE_THAT(src.GetIdtf(), Catch::Equals("c"));
      REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("d"));

      REQUIRE(src.GetType() == ScType::NodeConst);
      REQUIRE(trg.GetType() == ScType::NodeConst);
      REQUIRE(edge.GetType() == ScType::EdgeUCommon);
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"scs_comments\" failed")
  }
  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_level_1", "[test scs parser]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_1");

  SECTION("simple")
  {
    SUBTEST_START("simple")
    {
      try
      {
        char const * data = "sc_node#a | sc_edge#e1 | sc_node#b;;";
        scs::Parser parser(ctx);

        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 1);
        {
          auto const & src = parser.GetParsedElement(triples[0].m_source);
          auto const & edge = parser.GetParsedElement(triples[0].m_edge);
          auto const & trg = parser.GetParsedElement(triples[0].m_target);

          REQUIRE(src.GetType() == ScType::NodeConst);
          REQUIRE(trg.GetType() == ScType::NodeConst);
          REQUIRE(edge.GetType() == ScType::EdgeUCommonConst);
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"scs_comments\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_const_var", "[test scs parser]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_const_var");

  char const * data = "_a _-> b;;";
  scs::Parser parser(ctx);
  try
  {
    REQUIRE(parser.Parse(data));

    auto const & triples = parser.GetParsedTriples();
    REQUIRE(triples.size() == 1);

    {
      auto const & src = parser.GetParsedElement(triples[0].m_source);
      auto const & edge = parser.GetParsedElement(triples[0].m_edge);
      auto const & trg = parser.GetParsedElement(triples[0].m_target);

      REQUIRE(src.GetType() == ScType::NodeVar);
      REQUIRE(edge.GetType() == ScType::EdgeAccessVarPosPerm);
      REQUIRE(trg.GetType() == ScType::NodeConst);

      REQUIRE_THAT(src.GetIdtf(), Catch::Equals("_a"));
      REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("b"));
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"scs_const_var\" failed")
  }
  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_level_2", "[test scs parser]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_2");

  SECTION("simple_1")
  {
    SUBTEST_START("simple_1")
    {
      try
      {
        char const * data = "a -> (b <- c);;";

        scs::Parser parser(ctx);

        REQUIRE(parser.Parse(data));
        TripleTester tester(parser);
        tester(
              {
                    {
                          { ScType::NodeConst, "c" },
                          { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                          { ScType::NodeConst,              "b" }
                    },
                    {
                          { ScType::NodeConst, "a" },
                          { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                          { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local }
                    }
              });

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 2);
        REQUIRE(triples[0].m_edge == triples[1].m_target);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"simple_1\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("simple_2")
  {
    SUBTEST_START("simple_2")
    {
      try
      {
        char const * data = "(a -> b) => c;;";
        scs::Parser parser(ctx);

        REQUIRE(parser.Parse(data));
        TripleTester tester(parser);
        tester(
              {
                    {
                          { ScType::NodeConst,              "a" },
                          { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                          { ScType::NodeConst, "b" }
                    },
                    {
                          { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                          { ScType::EdgeDCommonConst,       "", scs::Visibility::Local },
                          { ScType::NodeConst, "c" }
                    }
              });

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 2);
        REQUIRE(triples[0].m_edge == triples[1].m_source);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"simple_2\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("complex")
  {
    SUBTEST_START("complex")
    {
      try
      {
        char const * data =
              "a <> (b -> c);;"
              "(c <- x) <- (b -> y);;";

        scs::Parser parser(ctx);
        SC_CHECK(parser.Parse(data), ());

        TripleTester tester(parser);
        tester({
                     {
                           { ScType::NodeConst,              "b" },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::NodeConst,              "c" }
                     },
                     {
                           { ScType::NodeConst,              "a" },
                           { ScType::EdgeUCommon,            "", scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local }
                     },
                     {
                           { ScType::NodeConst,              "x" },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::NodeConst,              "c" }
                     },
                     {
                           { ScType::NodeConst,              "b" },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::NodeConst,              "y" }
                     },
                     {
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, "", scs::Visibility::Local }
                     }
               });

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 5);

        REQUIRE(triples[0].m_edge == triples[1].m_target);
        REQUIRE(triples[2].m_edge == triples[4].m_target);
        REQUIRE(triples[3].m_edge == triples[4].m_source);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"complex\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_level_3", "[test scs parser]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_3");

  SECTION("simple_1")
  {
    SUBTEST_START("simple_1")
    {
      try
      {
        char const * data = "a -> c: _b:: d;;";
        scs::Parser parser(ctx);

        REQUIRE(parser.Parse(data));
        TripleTester tester(parser);
        tester({
                     {
                           { ScType::NodeConst, "a" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::NodeConst,              "d" }
                     },
                     {
                           { ScType::NodeConst, "c" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
                     },
                     {
                           { ScType::NodeVar,   "_b" },
                           { ScType::EdgeAccessVarPosPerm,   scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
                     }
               });

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 3);

        REQUIRE(triples[1].m_target == triples[0].m_edge);
        REQUIRE(triples[2].m_target == triples[0].m_edge);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"simple_1\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("complex_1")
  {
    SUBTEST_START("complex_1")
    {
      try
      {
        char const * data = "(a <- f: d) -> (c -> b: d);;";

        scs::Parser parser(ctx);
        REQUIRE(parser.Parse(data));

        TripleTester tester(parser);
        tester({
                     {
                           { ScType::NodeConst,              "d" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::NodeConst,              "a" }
                     },
                     {
                           { ScType::NodeConst,              "f" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
                     },
                     {
                           { ScType::NodeConst,              "c" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::NodeConst,              "d" }
                     },
                     {
                           { ScType::NodeConst,              "b" },
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
        REQUIRE(triples.size() == 5);

        REQUIRE(triples[1].m_target == triples[0].m_edge);
        REQUIRE(triples[3].m_target == triples[2].m_edge);
        REQUIRE(triples[4].m_source == triples[0].m_edge);
        REQUIRE(triples[4].m_target == triples[2].m_edge);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"complex_1\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("complex_2")
  {
    SUBTEST_START("complex_2")
    {
      try
      {
        char const * data = "a -> c: (d -> g: h);;";

        scs::Parser parser(ctx);
        REQUIRE(parser.Parse(data));

        TripleTester tester(parser);
        tester({
                     {
                           { ScType::NodeConst, "d" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::NodeConst,              "h" }
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
        REQUIRE(triples.size() == 4);

        REQUIRE(triples[0].m_edge == triples[1].m_target);
        REQUIRE(triples[2].m_target == triples[0].m_edge);
        REQUIRE(triples[3].m_target == triples[2].m_edge);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"complex_2\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_level_4", "[test scs parser]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_4");

  SECTION("simple_1")
  {
    SUBTEST_START("simple_1")
    {
      try
      {
        char const * data = "a -> b: c; d;;";

        scs::Parser parser(ctx);
        REQUIRE(parser.Parse(data));

        TripleTester tester(parser);
        tester({
                     {
                           { ScType::NodeConst, "a" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::NodeConst,              "c" }
                     },
                     {
                           { ScType::NodeConst, "b" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
                     },
                     {
                           { ScType::NodeConst, "a" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::NodeConst,              "d" }
                     },
                     {
                           { ScType::NodeConst, "b" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local }
                     }
               });

        auto const & triples = parser.GetParsedTriples();

        REQUIRE(triples.size() == 4);

        REQUIRE(triples[1].m_target == triples[0].m_edge);
        REQUIRE(triples[3].m_target == triples[2].m_edge);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"simple_1\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("simple_2")
  {
    SUBTEST_START("simple_2")
    {
      try
      {
        char const * data = "a -> b: c; <- d: e: f;;";

        scs::Parser parser(ctx);
        REQUIRE(parser.Parse(data));

        TripleTester tester(parser);
        tester({
                     {
                           { ScType::NodeConst, "a" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::NodeConst,              "c" }
                     },
                     {
                           { ScType::NodeConst, "b" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                     },
                     {
                           { ScType::NodeConst, "f" },
                           { ScType::EdgeAccessConstPosPerm, scs::Visibility::Local },
                           { ScType::NodeConst,              "a" }
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

        REQUIRE(triples.size() == 5);

        REQUIRE(triples[0].m_edge == triples[1].m_target);
        REQUIRE(triples[2].m_edge == triples[3].m_target);
        REQUIRE(triples[2].m_edge == triples[4].m_target);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"simple_2\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_types", "[test scs parser]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");

  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_types");

  SECTION("nodes")
  {
    SUBTEST_START("nodes")
    {
      try
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

        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 4);
        {
          auto const CheckSourceNode = [&triples, &parser](size_t index, ScType type)
          {
            REQUIRE(index < triples.size());
            return (parser.GetParsedElement(triples[index].m_source).GetType() == type);
          };

          auto const CheckTargetNode = [&triples, &parser](size_t index, ScType type)
          {
            REQUIRE(index < triples.size());
            return (parser.GetParsedElement(triples[index].m_target).GetType() == type);
          };

          REQUIRE(CheckSourceNode(0, ScType::NodeConstTuple));
          REQUIRE(CheckTargetNode(0, ScType::NodeConstStruct));
          REQUIRE(CheckSourceNode(1, ScType::NodeConstRole));
          REQUIRE(CheckTargetNode(1, ScType::NodeVarNoRole));
          REQUIRE(CheckSourceNode(2, ScType::NodeConstClass));
          REQUIRE(CheckTargetNode(2, ScType::NodeConstAbstract));
          REQUIRE(CheckSourceNode(3, ScType::NodeConstAbstract));
          REQUIRE(CheckTargetNode(3, ScType::NodeConstMaterial));
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"nodes\" failed")
      }

    }
    SUBTEST_END()
  }

  SECTION("links")
  {
    SUBTEST_START("links")
    {
      try
      {
        char const * data =
              "a -> \"file://data.txt\";;"
              "b -> [x];;"
              "c -> _[];;"
              "d -> [];;";
        scs::Parser parser(ctx);

        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();

        REQUIRE(triples.size() == 4);

        REQUIRE(parser.GetParsedElement(triples[0].m_target).GetType() == ScType::Link);
        REQUIRE(parser.GetParsedElement(triples[1].m_target).GetType() == ScType::LinkConst);
        REQUIRE(parser.GetParsedElement(triples[2].m_target).GetType() == ScType::LinkVar);
        REQUIRE(parser.GetParsedElement(triples[3].m_target).GetType() == ScType::LinkConst);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"links\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("backward_compatibility")
  {
    SUBTEST_START("backward_compatibility")
    {
      try
      {
        char const * data = "a <- c;; a <- sc_node_not_relation;; b <- c;; b <- sc_node_not_binary_tuple;;";
        scs::Parser parser(ctx);

        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 2);

        REQUIRE(parser.GetParsedElement(triples[0].m_target).GetType() == ScType::NodeConstClass);
        REQUIRE(parser.GetParsedElement(triples[1].m_target).GetType() == ScType::NodeConstTuple);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"backward_compatibility\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("edges")
  {
    SUBTEST_START("edges")
    {
      try
      {
        char const * data = "x"
                            "> _y; <> y4; ..> y5;"
                            "<=> y7; _<=> y8; => y9; _=> y11;"
                            "-> y2; _-> y13; -|> y15; _-|> y17; -/> y19; _-/> y21;"
                            " ~> y23; _~> y25; ~|> y27; _~|> y29; ~/> y31; _~/> y33;;";

        scs::Parser parser(ctx);

        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 19);
        {
          auto const CheckEdgeType = [&triples, &parser](size_t index, ScType type) -> bool
          {
            REQUIRE(index < triples.size());
            return (parser.GetParsedElement(triples[index].m_edge).GetType() == type);
          };

          REQUIRE(CheckEdgeType(0, ScType::EdgeDCommon));
          REQUIRE(CheckEdgeType(1, ScType::EdgeUCommon));
          REQUIRE(CheckEdgeType(2, ScType::EdgeAccess));

          REQUIRE(CheckEdgeType(3, ScType::EdgeUCommonConst));
          REQUIRE(CheckEdgeType(4, ScType::EdgeUCommonVar));
          REQUIRE(CheckEdgeType(5, ScType::EdgeDCommonConst));
          REQUIRE(CheckEdgeType(6, ScType::EdgeDCommonVar));

          REQUIRE(CheckEdgeType(7, ScType::EdgeAccessConstPosPerm));
          REQUIRE(CheckEdgeType(8, ScType::EdgeAccessVarPosPerm));
          REQUIRE(CheckEdgeType(9, ScType::EdgeAccessConstNegPerm));
          REQUIRE(CheckEdgeType(10, ScType::EdgeAccessVarNegPerm));
          REQUIRE(CheckEdgeType(11, ScType::EdgeAccessConstFuzPerm));
          REQUIRE(CheckEdgeType(12, ScType::EdgeAccessVarFuzPerm));

          REQUIRE(CheckEdgeType(13, ScType::EdgeAccessConstPosTemp));
          REQUIRE(CheckEdgeType(14, ScType::EdgeAccessVarPosTemp));
          REQUIRE(CheckEdgeType(15, ScType::EdgeAccessConstNegTemp));
          REQUIRE(CheckEdgeType(16, ScType::EdgeAccessVarNegTemp));
          REQUIRE(CheckEdgeType(17, ScType::EdgeAccessConstFuzTemp));
          REQUIRE(CheckEdgeType(18, ScType::EdgeAccessVarFuzTemp));
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"edges\" failed")
      }
    }
    SUBTEST_END()
  }

  SECTION("type_error")
  {
    SUBTEST_START("type_error")
    {
      try
      {
        char const * data = "a <- sc_node_abstract;; a <- sc_node_role_relation;;";

        scs::Parser parser(ctx);
        REQUIRE_FALSE(parser.Parse(data));
      } catch (...)
      {
        SC_LOG_ERROR("Test \"type_error\" failed")
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();

  test::ScTestUnit::ShutdownMemory(false);
}