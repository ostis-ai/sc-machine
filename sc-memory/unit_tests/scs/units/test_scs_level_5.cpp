/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/utils/sc_test.hpp"

#include "test_scs_utils.hpp"

UNIT_TEST(scs_level_5)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_5");

  SUBTEST_START(simple)
  {
    std::string const data = "set ~> attr:: item (* -/> subitem;; <= subitem2;; *);;";

    scs::Parser parser;

    SC_CHECK(parser.Parse(data), (parser.GetParseError()));

    TripleTester tester(parser);
    tester({
             {
               { ScType::NodeConst, "item" },
               { ScType::EdgeAccessConstFuzPerm, scs::Visibility::Local },
               { ScType::NodeConst, "subitem" }
             },
             {
               { ScType::NodeConst, "subitem2" },
               { ScType::EdgeDCommonConst, scs::Visibility::Local },
               { ScType::NodeConst, "item" }
             },
             {
               { ScType::NodeConst, "set" },
               { ScType::EdgeAccessConstPosTemp, scs::Visibility::Local },
               { ScType::NodeConst, "item" }
             },
             {
               { ScType::NodeConst, "attr" },
               { ScType::EdgeAccessVarPosPerm, scs::Visibility::Local },
               { ScType::EdgeAccessConstPosTemp, scs::Visibility::Local }
             }
           });

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 4, ());

    SC_CHECK_EQUAL(triples[3].m_target, triples[2].m_edge, ());
  }
  SUBTEST_END()
}
