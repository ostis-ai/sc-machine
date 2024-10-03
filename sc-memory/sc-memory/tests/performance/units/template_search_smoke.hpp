/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "template_test.hpp"

class TestTemplateSearchSmoke : public TestTemplate
{
public:
  void Setup(size_t constrCount) override
  {
    ScAddr const node = m_ctx->GenerateNode(ScType::ConstNode);
    for (uint32_t i = 0; i < constrCount; ++i)
    {
      ScAddr const trg = m_ctx->GenerateNode(ScType::ConstNodeStructure);
      m_ctx->GenerateConnector(ScType::ConstPermPosArc, node, trg);
    }

    m_templ.Triple(
          node,
          ScType::VarPermPosArc,
          ScType::VarNodeStructure);
  }
};
