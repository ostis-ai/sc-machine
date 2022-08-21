#pragma once

#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include "test_defines.hpp"

#include <memory>

class ScBuilderTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    ScBuilderTest::Initialize();
    m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "builder_test");
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

    params.clear = SC_FALSE;
    params.repo_path = SC_BUILDER_REPO_PATH;

    ScMemory::LogMute();
    ScMemory::Initialize(params);
    ScMemory::LogUnmute();
  }

  static void Shutdown()
  {
    ScMemory::LogMute();
    ScMemory::Shutdown(false);
    ScMemory::LogUnmute();
  }

protected:
  std::unique_ptr<ScMemoryContext> m_ctx;
};
