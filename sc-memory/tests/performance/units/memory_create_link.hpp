/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"

#include "sc-memory/sc_link.hpp"
#include <random>

class TestCreateLink : public TestMemory
{
public:
  void Run()
  {
    const int stringLength = 100;
    std::stringstream result;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> charDistribution(32, 126);

    for (int i = 0; i < stringLength; ++i)
    {
      char randomChar = charDistribution(gen);
      result << randomChar;
    }

    ScAddr const addr = m_ctx->CreateLink();

    ScLink link(*m_ctx, addr);
    BENCHMARK_BUILTIN_EXPECT(link.Set(result.str()), true);
  }
};

class TestCreateSameLink : public TestMemory
{
public:
  void Run()
  {
    ScAddr const addr = m_ctx->CreateLink();

    ScLink link(*m_ctx, addr);
    std::string const & content = "same content";
    BENCHMARK_BUILTIN_EXPECT(link.Set(content), true);
  }
};
