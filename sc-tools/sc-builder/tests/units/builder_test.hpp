/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/test/sc_test.hpp>

#include <memory>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_agent_context.hpp>
#include <algorithm>

class ScBuilderTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    ScBuilderTest::Initialize();
    m_ctx = std::make_unique<ScAgentContext>();
  }

  virtual void TearDown()
  {
    if (m_ctx)
      m_ctx->Destroy();

    ScBuilderTest::Shutdown();
  }

  static void Initialize()
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.dump_memory = SC_FALSE;
    params.dump_memory_statistics = SC_FALSE;

    params.clear = SC_FALSE;
    params.storage = SC_BUILDER_KB_BIN;

    ScMemory::LogMute();
    ScMemory::Initialize(params);
    ScMemory::LogUnmute();
  }

  void InitializeWithUserMode()
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.dump_memory = SC_FALSE;
    params.dump_memory_statistics = SC_FALSE;

    params.clear = SC_FALSE;
    params.storage = SC_BUILDER_KB_BIN;

    params.user_mode = SC_TRUE;

    ScMemory::LogMute();
    ScMemory::Initialize(params);
    ScMemory::LogUnmute();
  }

  static void Shutdown()
  {
    ScMemory::LogMute();
    ScMemory::Shutdown(SC_FALSE);
    ScMemory::LogUnmute();
  }

protected:
  std::unique_ptr<ScAgentContext> m_ctx;
};

class ScBuilderLoadUserPermissionsTest : public ScBuilderTest
{
protected:
  virtual void SetUp()
  {
    ScBuilderTest::InitializeWithUserMode();
    m_ctx = std::make_unique<TestScMemoryContext>(ScKeynodes::myself);
  }
};
