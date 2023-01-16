#pragma once

#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

#include <memory>

class ScMemoryTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    ScMemoryTest::Initialize();
    m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "test");
  }

  virtual void TearDown()
  {
    if (m_ctx)
      m_ctx->Destroy();

    ScMemoryTest::Shutdown();
  }

  void Initialize()
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.clear = SC_TRUE;
    params.repo_path = "repo";
    params.log_level = "Debug";

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

class ScMemoryTestWithInitMemoryGeneratedStructure : public ScMemoryTest
{
  virtual void SetUp()
  {
    ScMemoryTestWithInitMemoryGeneratedStructure::Initialize();
    m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "test");
  }

  void Initialize()
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.clear = SC_TRUE;
    params.repo_path = "repo";
    params.log_level = "Debug";

    params.init_memory_generated_upload = SC_TRUE;
    params.init_memory_generated_structure = "result_structure";

    ScMemory::LogMute();
    ScMemory::Initialize(params);
    ScMemory::LogUnmute();
  }
};
