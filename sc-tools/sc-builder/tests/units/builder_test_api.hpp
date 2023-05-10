#pragma once

#include <gtest/gtest.h>

#include "../../src/builder.hpp"

#include "test_defines.hpp"

#define TEST_REPOS_KB SC_BUILDER_TEST_REPOS "/kb"

class TestBuilder : public Builder
{
public:
  TestBuilder() = default;

  explicit TestBuilder(BuilderParams const & params)
  {
    m_params = params;
  }

  ScAddr TestResolveOutputStructure()
  {
    return Builder::ResolveOutputStructure();
  }

  bool TestBuildSources(std::unordered_set<std::string> const & buildSources, ScAddr const & outputStructure)
  {
    return Builder::BuildSources(buildSources, outputStructure);
  }

  bool TestProcessFile(std::string const & filename, ScAddr const & outputStructure)
  {
    return Builder::ProcessFile(filename, outputStructure);
  }

  bool TestIsSourceFile(std::string const & filePath)
  {
    return Builder::IsSourceFile(filePath);
  }

  bool TestIsRepoPathFile(std::string const & filePath) const
  {
    return Builder::IsRepoPathFile(filePath);
  }

  void TestParseRepoPath(
      std::string const & repoPath,
      std::unordered_set<std::string> & excludedSources,
      std::unordered_set<std::string> & checkSources) const
  {
    return Builder::ParseRepoPath(repoPath, excludedSources, checkSources);
  }

  void TestCollectBuildSources(
      std::string const & path,
      std::unordered_set<std::string> const & excludedSources,
      std::unordered_set<std::string> const & checkSources,
      std::unordered_set<std::string> & buildSources)
  {
    return Builder::CollectBuildSources(path, excludedSources, checkSources, buildSources);
  }
};

class ScBuilderTestAPI : public testing::Test
{
protected:
  TestBuilder builder;

  void SetUp() override
  {
    BuilderParams params;
    params.m_inputPath = TEST_REPOS_KB;
    params.m_outputPath = SC_BUILDER_REPO_PATH;
    params.m_autoFormatInfo = SC_TRUE;

    sc_memory_params memoryParams;
    sc_memory_params_clear(&memoryParams);
    memoryParams.repo_path = SC_BUILDER_REPO_PATH;

    builder = TestBuilder(params);
  }

  void TearDown() override
  {
  }
};
