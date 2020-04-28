/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-test-framework/sc_test_unit.hpp"

#include "test_scs_utils.hpp"

UNIT_TEST(scs_level_1)
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "scs_level_1");

  SUBTEST_START(simple)
  {
    char const * data = "sc_node#a | sc_edge#e1 | sc_node#b;;";
    scs::Parser parser;

    SC_CHECK(parser.Parse(data), ());

    auto const & triples = parser.GetParsedTriples();
    SC_CHECK_EQUAL(triples.size(), 1, ());
    {
      SPLIT_TRIPLE(triples[0]);

      SC_CHECK_EQUAL(src.GetType(), ScType::NodeConst, ());
      SC_CHECK_EQUAL(trg.GetType(), ScType::NodeConst, ());
      SC_CHECK_EQUAL(edge.GetType(), ScType::EdgeUCommonConst, ());
    }
  }
  SUBTEST_END()
}
