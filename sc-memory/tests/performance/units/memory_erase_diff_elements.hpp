/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"

#include <mutex>

class TestEraseDiffElements : public TestMemory
{
public:
  void Run()
  {
    ScAddr addr;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      addr = m_elements.back();
      m_elements.pop_back();
    }

    m_ctx->EraseElement(addr);
  }

  void Setup(size_t objectsNum) override
  {
    for (size_t i = 0; i < objectsNum; ++i)
      m_elements.push_back(m_ctx->GenerateNode(ScType::NodeConst));
  }

private:
  static ScAddrList m_elements;
  static std::mutex m_mutex;
};

ScAddrList TestEraseDiffElements::m_elements;
std::mutex TestEraseDiffElements::m_mutex;
