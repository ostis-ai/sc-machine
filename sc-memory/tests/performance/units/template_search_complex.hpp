/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "template_test.hpp"

class TestTemplateSearchComplex : public TestTemplate
{
public:
  void Setup(size_t constrCount) override
  {
    ScAddr const node = m_ctx->GenerateNode(ScType::NodeConstStruct);
    m_templ.Quintuple(
          node,
          ScType::EdgeDCommonVar,
          ScType::NodeVarTuple >> "_tuple",
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVarNoRole);
    m_templ.Quintuple(
          ScType::NodeVarClass,
          ScType::EdgeDCommonVar,
          "_tuple",
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVarNoRole);
    m_templ.Triple(
          ScType::NodeVarClass,
          ScType::EdgeAccessVarPosPerm,
          node);

    for (size_t i = 0; i < constrCount; ++i)
    {
      ScTemplateGenResult result;
      m_ctx->GenerateByTemplate(m_templ, result);
    }
  }
};
