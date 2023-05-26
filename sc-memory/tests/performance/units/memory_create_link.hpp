/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"

#include "sc-memory/sc_link.hpp"

class TestCreateLink : public TestMemory
{
public:
  void Run()
  {
    ScAddr const addr = m_ctx->CreateLink();

    ScLink link(*m_ctx, addr);
    BENCHMARK_BUILTIN_EXPECT(link.Set(addr.Hash()), SC_TRUE);
    BENCHMARK_BUILTIN_EXPECT(link.Get<sc_addr_hash>(), addr.Hash());
    BENCHMARK_BUILTIN_EXPECT(m_ctx->FindLinksByContent(addr.Hash()).size(), 1u);
    BENCHMARK_BUILTIN_EXPECT(m_ctx->FindLinksByContentSubstring(addr.Hash()).size(), 1u);
    BENCHMARK_BUILTIN_EXPECT(m_ctx->FindLinksContentsByContentSubstring(addr.Hash()).size(), 1u);
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
    BENCHMARK_BUILTIN_EXPECT(link.Set(content), SC_TRUE);
    BENCHMARK_BUILTIN_EXPECT(link.Get<std::string>() == content, SC_TRUE);
    BENCHMARK_BUILTIN_EXPECT(m_ctx->FindLinksByContent(content).size(), SC_TRUE);
  }
};
