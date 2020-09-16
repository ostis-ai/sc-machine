/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "test_scs_utils.hpp"

TEST_CASE("scs_ElementHandle", "[test scs common]")
{
  scs::ElementHandle handle_err;
  REQUIRE_FALSE(handle_err.IsValid());
  REQUIRE_FALSE(handle_err.IsLocal());

  scs::ElementHandle handle_ok(1);
  REQUIRE(handle_ok.IsValid());
  REQUIRE_FALSE(handle_ok.IsLocal());

  scs::ElementHandle handle_local(0, true);
  REQUIRE(handle_local.IsValid());
  REQUIRE(handle_local.IsLocal());
}

TEST_CASE("scs_parser_error", "[test scs common]")
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

        scs::Parser parser;

        REQUIRE_FALSE(parser.Parse(data));
        SC_LOG_WARNING(parser.GetParseError());
      } catch (...)
      {
        SC_LOG_ERROR("Test \"error_1\" failed");
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_parser_triple", "[test scs common]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_parser_triple");

  SECTION("triple_1")
  {
    SUBTEST_START("triple_1")
    {
      try
      {
        scs::Parser parser;
        char const * data = "a -> b;;";
        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 1);

        SPLIT_TRIPLE(triples[0]);

        REQUIRE(src.GetType() == ScType::NodeConst);
        REQUIRE(edge.GetType() == ScType::EdgeAccessConstPosPerm);
        REQUIRE(trg.GetType() == ScType::NodeConst);

        REQUIRE_THAT(src.GetIdtf(), Catch::Equals("a"));
        REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("b"));

        REQUIRE(src.GetVisibility() == scs::Visibility::System);
        REQUIRE(trg.GetVisibility() == scs::Visibility::System);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"triple_1\" failed");
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
        scs::Parser parser;
        char const * data = "a <- b;;";
        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 1);

        SPLIT_TRIPLE(triples[0]);

        REQUIRE_THAT(src.GetIdtf(), Catch::Equals("b"));
        REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("a"));
        REQUIRE(edge.GetType() == ScType::EdgeAccessConstPosPerm);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"reversed_1\" failed");
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
        scs::Parser parser;
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
          SPLIT_TRIPLE(triples[1]);

          REQUIRE_THAT(src.GetIdtf(), Catch::Equals("r"));
          REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("x"));
          REQUIRE(edge.GetType() == ScType::EdgeDCommonConst);
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"sentences_1\" failed");
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_comments", "[test scs common]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_comments");
  scs::Parser parser;

  try
  {
    char const * data =
          "//Level1\n"
          "a -> b;;/* example */\n"
          "c <> d;;";

    REQUIRE(parser.Parse(data));

    auto const & triples = parser.GetParsedTriples();
    REQUIRE(triples.size() == 2);

    {
      SPLIT_TRIPLE(triples[0]);

      REQUIRE_THAT(src.GetIdtf(), Catch::Equals("a"));
      REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("b"));

      REQUIRE(src.GetType() == ScType::NodeConst);
      REQUIRE(trg.GetType() == ScType::NodeConst);
      REQUIRE(edge.GetType() == ScType::EdgeAccessConstPosPerm);
    }

    {
      SPLIT_TRIPLE(triples[1]);

      REQUIRE_THAT(src.GetIdtf(), Catch::Equals("c"));
      REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("d"));

      REQUIRE(src.GetType() == ScType::NodeConst);
      REQUIRE(trg.GetType() == ScType::NodeConst);
      REQUIRE(edge.GetType() == ScType::EdgeUCommon);
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"scs_comments\" failed");
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_const_var", "[test scs common]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_const_var");

  try
  {
    char const * data = "_a _-> b;;";
    scs::Parser parser;

    REQUIRE(parser.Parse(data));

    auto const & triples = parser.GetParsedTriples();
    REQUIRE(triples.size() == 1);

    {
      SPLIT_TRIPLE(triples[0]);

      REQUIRE(src.GetType() == ScType::NodeVar);
      REQUIRE(edge.GetType() == ScType::EdgeAccessVarPosPerm);
      REQUIRE(trg.GetType() == ScType::NodeConst);

      REQUIRE_THAT(src.GetIdtf(), Catch::Equals("_a"));
      REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("b"));
    }
  } catch (...)
  {
    SC_LOG_ERROR("Test \"scs_const_var\" failed");
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_types", "[test scs common]")
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

        scs::Parser parser;

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
        SC_LOG_ERROR("Test \"nodes\" failed");
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
        std::string const data =
              "a -> \"file://data.txt\";;"
              "b -> [x];;"
              "c -> _[];;"
              "d -> [];;";
        scs::Parser parser;

        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();

        REQUIRE(triples.size() == 4);

        REQUIRE(parser.GetParsedElement(triples[0].m_target).GetType() == ScType::LinkConst);
        REQUIRE(parser.GetParsedElement(triples[0].m_target).IsURL());
        REQUIRE(parser.GetParsedElement(triples[1].m_target).GetType() == ScType::LinkConst);
        REQUIRE_FALSE(parser.GetParsedElement(triples[2].m_target).IsURL());
        REQUIRE(parser.GetParsedElement(triples[2].m_target).GetType() == ScType::LinkVar);
        REQUIRE(parser.GetParsedElement(triples[3].m_target).GetType() == ScType::LinkConst);
        REQUIRE_FALSE(parser.GetParsedElement(triples[3].m_target).IsURL());
      } catch (...)
      {
        SC_LOG_ERROR("Test \"links\" failed");
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
        std::string const data = "a <- c;; a <- sc_node_not_relation;; b <- c;; b <- sc_node_not_binary_tuple;;";
        scs::Parser parser;

        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 2);

        REQUIRE(parser.GetParsedElement(triples[0].m_target).GetType() == ScType::NodeConstClass);
        REQUIRE(parser.GetParsedElement(triples[1].m_target).GetType() == ScType::NodeConstTuple);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"backward_compatibility\" failed");
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
        std::string const data = "x"
                                 "> _y; <> y4; ..> y5;"
                                 "<=> y7; _<=> y8; => y9; _=> y11;"
                                 "-> y2; _-> y13; -|> y15; _-|> y17; -/> y19; _-/> y21;"
                                 " ~> y23; _~> y25; ~|> y27; _~|> y29; ~/> y31; _~/> y33;;";

        scs::Parser parser;

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
        SC_LOG_ERROR("Test \"edges\" failed");
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
        std::string const data = "a <- sc_node_abstract;; a <- sc_node_role_relation;;";

        scs::Parser parser;
        REQUIRE_FALSE(parser.Parse(data));
      } catch (...)
      {
        SC_LOG_ERROR("Test \"type_error\" failed");
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_aliases", "[test scs common]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_aliases");

  SECTION("simple_assign")
  {
    SUBTEST_START("simple_assign")
    {
      try
      {
        std::string const data = "@alias = [];; x ~> @alias;;";
        scs::Parser parser;

        REQUIRE(parser.Parse(data));
        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 1);

        auto const & t = triples.front();
        REQUIRE(parser.GetParsedElement(t.m_source).GetType().IsNode());
        REQUIRE(parser.GetParsedElement(t.m_edge).GetType() == ScType::EdgeAccessConstPosTemp);
        REQUIRE(parser.GetParsedElement(t.m_target).GetType().IsLink());
      } catch (...)
      {
        SC_LOG_ERROR("Test \"simple_assign\" failed");
      }
    }
    SUBTEST_END()
  }

  SECTION("no_assign")
  {
    SUBTEST_START("no_assign")
    {
      try
      {
        std::string const data = "x -> @y;;";
        scs::Parser parser;
        REQUIRE_FALSE(parser.Parse(data));
      } catch (...)
      {
        SC_LOG_ERROR("Test \"no_assign\" failed");
      }
    }
    SUBTEST_END()
  }

  SECTION("recursive_assigns")
  {
    SUBTEST_START("recursive_assigns")
    {
      try
      {
        std::string const data = "@alias1 = x;; @alias1 <- sc_node_tuple;; @alias2 = @alias1;; _y -|> x;;";

        scs::Parser parser;

        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 1);

        auto const t = triples.front();
        auto const & src = parser.GetParsedElement(t.m_source);
        auto const & edge = parser.GetParsedElement(t.m_edge);
        auto const & trg = parser.GetParsedElement(t.m_target);

        REQUIRE_THAT(src.GetIdtf(), Catch::Equals("_y"));
        REQUIRE(src.GetType().IsNode());
        REQUIRE(src.GetType().IsVar());
        REQUIRE(edge.GetType() == ScType::EdgeAccessConstNegPerm);
        REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("x"));
        REQUIRE(trg.GetType() == ScType::NodeConstTuple);
      } catch (...)
      {
        SC_LOG_ERROR("Test \"recursive_assigns\" failed");
      }
    }
    SUBTEST_END()
  }

  SECTION("alias_reassign")
  {
    SUBTEST_START("alias_reassign")
    {
      try
      {
        std::string const data = "@alias = _x;; _x <- sc_node_struct;; y _~/> @alias;; @alias = _[];; z _~> @alias;;";

        scs::Parser parser;

        REQUIRE(parser.Parse(data));

        auto const & triples = parser.GetParsedTriples();
        REQUIRE(triples.size() == 2);

        {
          auto const & t = triples[0];

          auto const & src = parser.GetParsedElement(t.m_source);
          auto const & edge = parser.GetParsedElement(t.m_edge);
          auto const & trg = parser.GetParsedElement(t.m_target);

          REQUIRE_THAT(src.GetIdtf(), Catch::Equals("y"));
          REQUIRE(src.GetType() == ScType::NodeConst);

          REQUIRE(edge.GetType() == ScType::EdgeAccessVarFuzTemp);

          REQUIRE_THAT(trg.GetIdtf(), Catch::Equals("_x"));
          REQUIRE(trg.GetType() == ScType::NodeVarStruct);
        }

        {
          auto const & t = triples[1];

          auto const & src = parser.GetParsedElement(t.m_source);
          auto const & edge = parser.GetParsedElement(t.m_edge);
          auto const & trg = parser.GetParsedElement(t.m_target);

          REQUIRE_THAT(src.GetIdtf(), Catch::Equals("z"));
          REQUIRE(src.GetType() == ScType::NodeConst);

          REQUIRE(edge.GetType() == ScType::EdgeAccessVarPosTemp);

          REQUIRE(trg.GetType() == ScType::LinkVar);
        }
      } catch (...)
      {
        SC_LOG_ERROR("Test \"alias_reassign\" failed");
      }
    }
    SUBTEST_END()
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}

TEST_CASE("scs_file_url", "[test scs common]")
{
  test::ScTestUnit::InitMemory("sc-memory.ini", "");
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_file_url");

  try
  {
    std::string const data = "scs_file_url -> \"file://html/faq.html\";;";
    scs::Parser parser;

    REQUIRE(parser.Parse(data));
    auto const & triples = parser.GetParsedTriples();
    REQUIRE(triples.size() == 1);

    SPLIT_TRIPLE(triples[0]);

    REQUIRE(trg.GetType() == ScType::LinkConst);
    REQUIRE_THAT(trg.GetValue(), Catch::Equals("file://html/faq.html"));
    REQUIRE(trg.IsURL());
  } catch (...)
  {
    SC_LOG_ERROR("Test \"scs_file_url\" failed");
  }

  ctx.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
}
