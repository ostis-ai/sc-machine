/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"

class TestGenerateConnector : public TestMemory
{
public:
  void Run()
  {
    m_ctx->GenerateConnector(
          ScType::ConstPermPosArc,
          m_nodes[random() % m_nodes.size()],
          m_nodes[random() % m_nodes.size()]);
  }

  void Setup(size_t elementsNum) override
  {
    m_nodes.reserve(elementsNum);
    for (size_t i = 0; i < elementsNum; ++i)
      m_nodes.push_back(m_ctx->GenerateNode(ScType::ConstNode));
  }

private:
  static ScAddrVector m_nodes;
};

ScAddrVector TestGenerateConnector::m_nodes;
