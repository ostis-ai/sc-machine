/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"

class TestIteratorSearch : public TestMemory
{
public:
  void Run()
  {
    if (!it)
    {
      it = m_ctx->Iterator3(m_node, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    }

    BENCHMARK_BUILTIN_EXPECT(it->Next(), SC_TRUE);
  }

  void Setup(size_t edgesNum) override
  {
    m_node = m_ctx->CreateNode(ScType::NodeConstClass);
    for (size_t i = 0; i < edgesNum; ++i)
    {
      ScAddr target = m_ctx->CreateNode(ScType::NodeConst);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_node, target);
    }
  }

private:
  ScIterator3Ptr it;
  static ScAddr m_node;
};

ScAddr TestIteratorSearch::m_node;
