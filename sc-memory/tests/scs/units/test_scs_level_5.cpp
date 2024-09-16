/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <gtest/gtest.h>

#include "test_scs_utils.hpp"

TEST(scs_level_5, simple)
{
  std::string const data = "set ~> attr:: item (* -/> subitem;; <= subitem2;; *);;";

  scs::Parser parser;

  EXPECT_TRUE(parser.Parse(data));

  TripleTester tester(parser);
  tester({
           {
             { ScType::ConstNode, "item" },
             { ScType::ConstFuzArc, scs::Visibility::Local },
             { ScType::ConstNode, "subitem" }
           },
           {
             { ScType::ConstNode, "subitem2" },
             { ScType::ConstCommonArc, scs::Visibility::Local },
             { ScType::ConstNode, "item" }
           },
           {
             { ScType::ConstNode, "set" },
             { ScType::ConstTempPosArc, scs::Visibility::Local },
             { ScType::ConstNode, "item" }
           },
           {
             { ScType::ConstNode, "attr" },
             { ScType::VarPermPosArc, scs::Visibility::Local },
             { ScType::ConstTempPosArc, scs::Visibility::Local }
           }
         });

  auto const & triples = parser.GetParsedTriples();
  EXPECT_EQ(triples.size(), 4u);

  EXPECT_EQ(triples[3].m_target, triples[2].m_connector);
}
