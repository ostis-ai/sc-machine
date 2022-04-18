#pragma once

#include "gtest/gtest.h"

#include "sc-memory/sc_memory.hpp"

#include <memory>

class GenerationByTemplateTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    GenerationByTemplateTest::Initialize();
    m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "sc_agents_common_test");
  }

  virtual void TearDown()
  {
    if (m_ctx)
      m_ctx->Destroy();

    GenerationByTemplateTest::Shutdown();
  }

  static void Initialize()
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.clear = SC_TRUE;
    params.repo_path = SC_AGENTS_COMMON_REPO_PATH;

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