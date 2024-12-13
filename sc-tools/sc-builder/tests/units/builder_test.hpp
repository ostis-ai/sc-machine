/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/test/sc_test.hpp>

#include <memory>
#include <algorithm>
#include <unordered_set>
#include <filesystem>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_agent_context.hpp>

#include <sc-builder/scs_loader.hpp>

class ScBuilderTest : public testing::Test
{
public:
  static inline std::string const & SC_BUILDER_INI = "../sc-builder-test.ini";
  static inline std::string const & SC_BUILDER_KB_BIN = "sc-builder-test-kb-bin";
  static inline std::string const & SC_BUILDER_REPO_PATH = "../repo.path";
  static inline std::string const & SC_BUILDER_KB = "../kb";
  static inline std::string const & SC_BUILDER_TEST_REPOS = "../repos";
  static inline std::string const & SC_BUILDER_KB_GWF = SC_BUILDER_KB + "/tests-gwf-to-scs/";
  static inline std::string const & SC_BUILDER_CONFIGS = "../configs";

protected:
  void LoadKB(std::unique_ptr<ScAgentContext> const & m_context, std::unordered_set<std::string> const & sources)
  {
    ScsLoader loader;
    for (std::string const & source : sources)
      loader.loadScsFile(*m_context, ScBuilderTest::SC_BUILDER_KB + "/" + source);
  }

  virtual void SetUp()
  {
    ScBuilderTest::Initialize();
    m_ctx = std::make_unique<ScAgentContext>();
  }

  virtual void TearDown()
  {
    if (m_ctx)
      m_ctx->Destroy();

    ScBuilderTest::Shutdown();

    std::filesystem::remove_all(SC_BUILDER_KB_BIN);
  }

  static void Initialize()
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.dump_memory = SC_FALSE;
    params.dump_memory_statistics = SC_FALSE;

    params.clear = SC_TRUE;
    params.storage = SC_BUILDER_KB_BIN.c_str();

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
    params.storage = SC_BUILDER_KB_BIN.c_str();

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
  std::unique_ptr<ScAgentContext> m_ctx;
};

class ScBuilderLoadUserPermissionsTest : public ScBuilderTest
{
protected:
  virtual void SetUp()
  {
    ScBuilderTest::InitializeWithUserMode();
    m_ctx = std::make_unique<TestScMemoryContext>(ScKeynodes::myself);
  }
};
