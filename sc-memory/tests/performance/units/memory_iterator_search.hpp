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
      it = m_ctx->CreateIterator3(m_node, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    }

    BENCHMARK_BUILTIN_EXPECT(it->Next(), true);
  }

  void Setup(size_t connectorsNum) override
  {
    m_node = m_ctx->GenerateNode(ScType::NodeConstClass);
    for (size_t i = 0; i < connectorsNum; ++i)
    {
      ScAddr target = m_ctx->GenerateNode(ScType::NodeConst);
      m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, m_node, target);
    }
  }

private:
  ScIterator3Ptr it;
  static ScAddr m_node;
};

ScAddr TestIteratorSearch::m_node;
