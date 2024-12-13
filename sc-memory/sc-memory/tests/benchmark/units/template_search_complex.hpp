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
    ScAddr const node = m_ctx->GenerateNode(ScType::ConstNodeStructure);
    m_templ.Quintuple(
          node,
          ScType::VarCommonArc,
          ScType::VarNodeTuple >> "_tuple",
          ScType::VarPermPosArc,
          ScType::VarNodeNonRole);
    m_templ.Quintuple(
          ScType::VarNodeClass,
          ScType::VarCommonArc,
          "_tuple",
          ScType::VarPermPosArc,
          ScType::VarNodeNonRole);
    m_templ.Triple(
          ScType::VarNodeClass,
          ScType::VarPermPosArc,
          node);

    for (size_t i = 0; i < constrCount; ++i)
    {
      ScTemplateGenResult result;
      m_ctx->GenerateByTemplate(m_templ, result);
    }
  }
};
