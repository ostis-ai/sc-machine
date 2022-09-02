/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "gtest/gtest.h"

#include "sc-memory/sc_memory.hpp"

#include "../../test_defines.hpp"

#include "../../../src/sc-server-impl/sc_server_impl.hpp"

#include <memory>

class ScServerTest : public testing::Test
{
protected:
  void SetUp() override
  {
    Initialize();
    m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "sc_server_test");
  }

  void TearDown() override
  {
    if (m_ctx != nullptr)
      m_ctx->Destroy();

    Shutdown();
  }

  void Initialize()
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.clear = SC_FALSE;
    params.repo_path = SC_SERVER_REPO_PATH;

    ScMemory::LogMute();
    m_server =
        std::make_unique<ScServerImpl>("127.0.0.1", 8865, "File", "test-sc-server.log", "Error", SC_TRUE, params);
    m_server->SetMessageChannels(ScServerLogMessages::all);
    m_server->SetErrorChannels(ScServerLogErrors::all);
    m_server->Run();
    ScMemory::LogUnmute();
  }

  void Shutdown()
  {
    ScMemory::LogMute();
    m_server->Stop();
    m_server = nullptr;
    ScMemory::LogUnmute();
  }

protected:
  std::unique_ptr<ScMemoryContext> m_ctx;
  std::unique_ptr<ScServer> m_server;
};
