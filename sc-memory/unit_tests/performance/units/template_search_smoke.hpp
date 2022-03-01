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
    ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
    for (uint32_t i = 0; i < constrCount; ++i)
    {
      ScAddr const trg = m_ctx->CreateNode(ScType::NodeConstStruct);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, trg);
    }

    m_templ.Triple(
          node,
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVarStruct);
  }
};
