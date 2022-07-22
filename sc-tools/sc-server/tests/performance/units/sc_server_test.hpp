/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-core/sc_memory_params.h"

#include "../../test_defines.hpp"

#include "../../../src/sc-server-impl/sc_server_impl.hpp"
#include "../../sc_client.hpp"

class TestScServer
{
public:
  void Initialize(size_t objectsNum = 0)
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);
    params.repo_path = SC_SERVER_REPO_PATH;

    params.clear = SC_FALSE;

    ScMemory::LogMute();

    std::random_device random_device;
    std::uniform_int_distribution<> distribution(MIN_TEST_SERVER_PORT, MAX_TEST_SERVER_PORT);
    std::mt19937 generator(random_device());

    m_server = std::make_unique<ScServerImpl>(
        "127.0.0.1", distribution(generator), "File", "test-sc-server-bench.log", "Error", SC_TRUE, params);
    m_server->SetMessageChannels(ScServerLogMessages::all);
    m_server->SetErrorChannels(ScServerLogErrors::all);
    m_server->Run();

    ScMemory::LogUnmute();

    InitContext();
    Setup(objectsNum);
  }

  void Shutdown()
  {
    DestroyContext();

    ScMemory::LogMute();
    m_server->Stop();
    m_server = nullptr;
    ScMemory::LogUnmute();
  }

  void InitContext()
  {
    m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "sc_server_bench_test");
  }

  void DestroyContext()
  {
    if (m_ctx != nullptr)
      m_ctx->Destroy();
  }

  void WaitServer()
  {
    while (m_server->IsWorkable())
      ;
  }

  virtual void Setup(size_t objectsNum)
  {
  }

  static std::unique_ptr<ScServer> m_server;

protected:
  static std::unique_ptr<ScMemoryContext> m_ctx;

  sc_int const MAX_TEST_SERVER_PORT = 30000;
  sc_int const MIN_TEST_SERVER_PORT = 20000;
};

std::unique_ptr<ScServer> TestScServer::m_server{};
std::unique_ptr<ScMemoryContext> TestScServer::m_ctx{};
