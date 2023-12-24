#pragma once

#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

class ScMemoryTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    ScMemoryTest::Initialize();
    m_ctx = std::make_unique<ScMemoryContext>("test");
  }

  virtual void TearDown()
  {
    if (m_ctx)
      m_ctx->Destroy();

    ScMemoryTest::Shutdown();
  }

  void Initialize(sc_uint32 max_events_and_agents_threads = DEFAULT_MAX_EVENTS_AND_AGENTS_THREADS, std::string const & result_structure = "")
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.dump_memory = SC_FALSE;
    params.dump_memory_statistics = SC_FALSE;

    params.clear = SC_TRUE;
    params.repo_path = "repo";
    params.log_level = "Debug";

    params.max_events_and_agents_threads = max_events_and_agents_threads;

    params.init_memory_generated_upload = !result_structure.empty();
    params.init_memory_generated_structure = result_structure.c_str();

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
    ScMemoryTestWithInitMemoryGeneratedStructure::Initialize(DEFAULT_MAX_EVENTS_AND_AGENTS_THREADS, "result_structure");
    m_ctx = std::make_unique<ScMemoryContext>("test");
  }
};

class ScSingleThreadedMemoryTest : public ScMemoryTest
{
  virtual void SetUp()
  {
    ScSingleThreadedMemoryTest::Initialize(DEFAULT_MAX_EVENTS_AND_AGENTS_THREADS);
    m_ctx = std::make_unique<ScMemoryContext>("test");
  }
};
