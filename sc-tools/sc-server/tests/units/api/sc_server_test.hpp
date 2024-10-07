/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/test/sc_test.hpp>

#include <memory>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_keynodes.hpp>

#include "test_defines.hpp"

#include "sc-server-impl/sc_server_impl.hpp"

class ScServerTest : public testing::Test
{
protected:
  void SetUp() override
  {
    Initialize(SC_TRUE);
    m_ctx = std::make_unique<ScMemoryContext>();
  }

  void TearDown() override
  {
    if (m_ctx != nullptr)
      m_ctx->Destroy();

    Shutdown();
  }

  void Initialize(sc_bool parallel_actions)
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.dump_memory = SC_FALSE;
    params.dump_memory_statistics = SC_FALSE;

    params.clear = SC_FALSE;
    params.repo_path = SC_SERVER_REPO_PATH;

    ScMemory::LogMute();
    ScMemory::Initialize(params);
    m_server = std::make_unique<ScServerImpl>("127.0.0.1", 8865, parallel_actions);
    m_server->ClearChannels();
    m_server->Run();
    ScMemory::LogUnmute();
  }

  void Shutdown()
  {
    ScMemory::LogMute();
    m_server->Stop();
    m_server = nullptr;
    ScMemory::Shutdown();
    ScMemory::LogUnmute();
  }

protected:
  std::unique_ptr<ScMemoryContext> m_ctx;
  std::unique_ptr<ScServer> m_server;
};

class ScServerTestWithoutParallelMode : public ScServerTest
{
protected:
  void SetUp() override
  {
    Initialize(SC_FALSE);
    m_ctx = std::make_unique<ScMemoryContext>();
  }
};
