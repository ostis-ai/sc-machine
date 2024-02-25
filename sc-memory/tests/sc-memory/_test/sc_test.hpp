#pragma once

#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_keynodes.hpp"

class ScMemoryTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    ScMemoryTest::Initialize();
    m_ctx = std::make_unique<ScMemoryContext>();
  }

  virtual void TearDown()
  {
    if (m_ctx)
      m_ctx->Destroy();

    ScMemoryTest::Shutdown();
  }

  void Initialize(std::string const & result_structure = "")
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.dump_memory = SC_FALSE;
    params.dump_memory_statistics = SC_FALSE;

    params.clear = SC_TRUE;
    params.repo_path = "repo";
    params.log_level = "Debug";

    params.init_memory_generated_upload = !result_structure.empty();
    params.init_memory_generated_structure = result_structure.c_str();

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

    params.clear = SC_TRUE;
    params.repo_path = "repo";
    params.log_level = "Debug";

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
  std::unique_ptr<ScMemoryContext> m_ctx;
};

class ScMemoryTestWithInitMemoryGeneratedStructure : public ScMemoryTest
{
  virtual void SetUp()
  {
    ScMemoryTestWithInitMemoryGeneratedStructure::Initialize("result_structure");
    m_ctx = std::make_unique<ScMemoryContext>();
  }
};

class ScMemoryTestWithUserMode : public ScMemoryTest
{
  virtual void SetUp()
  {
    ScMemoryTestWithUserMode::InitializeWithUserMode();
    m_ctx = std::make_unique<ScMemoryContext>(ScKeynodes::kMySelf);
  }
};
