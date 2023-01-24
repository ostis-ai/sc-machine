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

class ScMemoryTestWithResultStruct : public ScMemoryTest
{
  virtual void SetUp()
  {
    ScMemoryTestWithResultStruct::Initialize();
    m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "test");
  }

  void Initialize()
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.clear = SC_TRUE;
    params.repo_path = "repo";
    params.log_level = "Debug";

    params.result_structure_upload = SC_TRUE;
    params.result_structure_system_idtf = "resultStructure";

    ScMemory::LogMute();
    ScMemory::Initialize(params);
    ScMemory::LogUnmute();
  }
};