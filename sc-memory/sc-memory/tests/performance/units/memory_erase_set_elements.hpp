/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"

#include <mutex>

class TestEraseSetElements : public TestMemory
{
public:
  void Run()
  {
    ScAddr addr;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      addr = m_connectors.back();
      m_connectors.pop_back();
    }

    m_ctx->EraseElement(addr);
  }

  void Setup(size_t objectsNum) override
  {
    m_addr = m_ctx->GenerateNode(ScType::ConstNode);
    for (size_t i = 0; i < objectsNum; ++i)
    {
      ScAddr target = m_ctx->GenerateNode(ScType::ConstNode);
      m_connectors.push_back(m_ctx->GenerateConnector(ScType::ConstPermPosArc, m_addr, target));
    }
  }

private:
  ScAddr m_addr;
  static std::mutex m_mutex;
  static ScAddrList m_connectors;
};

ScAddrList TestEraseSetElements::m_connectors;
std::mutex TestEraseSetElements::m_mutex;
