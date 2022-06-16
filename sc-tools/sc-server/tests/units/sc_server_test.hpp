#pragma once

#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "test_defines.hpp"

#include "../../src/sc-server-impl/sc_server_impl.hpp"

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

    m_server = std::make_unique<ScServerImpl>("localhost", 8765, "", params);
    m_server->Run();
  }

  void Shutdown()
  {
    m_server->Stop();
  }

protected:
  std::unique_ptr<ScMemoryContext> m_ctx;
  std::unique_ptr<ScServer> m_server;
};
