/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"

#include "sc-memory/sc_link.hpp"
#include <random>
#include <mutex>

class TestSearchLinkByContent : public TestMemory
{
public:
  void Run()
  {
    std::string content;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      content = m_contents.back();
      m_contents.pop_back();
    }

    BENCHMARK_BUILTIN_EXPECT(m_ctx->FindLinksByContent(content).empty(), false);
  }

  void Setup(size_t objectsNum) override
  {
    for (size_t i = 0; i < objectsNum; ++i)
    {
      const int stringLength = 100;
      std::stringstream result;

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<int> charDistribution(32, 126);

      for (size_t j = 0; j < stringLength; ++j)
      {
        char randomChar = charDistribution(gen);
        result << randomChar;
      }

      ScAddr const addr = m_ctx->CreateLink();

      std::string content = result.str();
      ScLink link(*m_ctx, addr);
      BENCHMARK_BUILTIN_EXPECT(link.Set(result.str()), true);

      m_contents.push_back(content);
    }
  }

private:
  static std::list<std::string> m_contents;
  static std::mutex m_mutex;
};

std::list<std::string> TestSearchLinkByContent::m_contents;
std::mutex TestSearchLinkByContent::m_mutex;
