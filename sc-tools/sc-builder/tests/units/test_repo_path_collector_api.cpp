/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include "sc-memory/sc_utils.hpp"
#include "repo_path_collector_test_api.hpp"

#define TEST_SIMPLE_REPO SC_BUILDER_TEST_REPOS "/simple_repo.path"
#define TEST_REPEATED_REPOS SC_BUILDER_TEST_REPOS "/repeated_repos.path"
#define TEST_EXCLUDED_REPOS SC_BUILDER_TEST_REPOS "/excluded_repos.path"
#define TEST_NON_TRIM_REPOS SC_BUILDER_TEST_REPOS "/non_trim_repos.path"
#define TEST_INVALID_REPO_PATH SC_BUILDER_TEST_REPOS "/invalid_repo_path"

TEST_F(ScRepoPathCollectorTestAPI, IsSources)
{
  std::string const & directory = TEST_REPOS_KB;

  EXPECT_TRUE(collector.IsSourceFile(directory + "/child_repos/child_kb/samples/sample1.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/child_repos/child_kb/samples/sample2.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/child_repos/child_kb/samples/sample3.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/samples/sample1.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/samples/sample2.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/samples/sample3.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/templates/template1.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/templates/template2.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/templates/template3.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/kb.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/rules.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/specification.scs"));
  EXPECT_TRUE(collector.IsSourceFile(directory + "/example.gwf"));
  EXPECT_FALSE(collector.IsSourceFile(directory + "/invalid_file"));
}

TEST_F(ScRepoPathCollectorTestAPI, IsExcludedPath)
{
  EXPECT_TRUE(collector.IsExcludedPath("!/child_repos/child_kb/samples/sample2.scs"));
  EXPECT_FALSE(collector.IsExcludedPath("/child_repos/child_kb/samples/sample1.scs"));
}

TEST_F(ScRepoPathCollectorTestAPI, IsRepoPath)
{
  EXPECT_TRUE(collector.IsRepoPathFile(TEST_SIMPLE_REPO));
  EXPECT_FALSE(collector.IsRepoPathFile(TEST_INVALID_REPO_PATH));
}

TEST_F(ScRepoPathCollectorTestAPI, SimpleRepo)
{
  std::string const & repoPath = TEST_SIMPLE_REPO;

  ScRepoPathCollector::Sources excludedSources, checkSources;
  EXPECT_TRUE(collector.IsRepoPathFile(repoPath));

  collector.ParseRepoPath(repoPath, excludedSources, checkSources);

  EXPECT_TRUE(excludedSources.empty());

  EXPECT_EQ(checkSources.size(), 1u);
  std::string const & directory = TEST_REPOS_KB;
  EXPECT_EQ(checkSources.count(directory), 1u);

  ScRepoPathCollector::Sources buildSources;
  collector.CollectBuildSources(repoPath, excludedSources, checkSources, buildSources);

  EXPECT_EQ(buildSources.size(), 13u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/templates/template1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/templates/template2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/templates/template3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/kb.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/rules.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/specification.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/example.gwf"), 1u);
}

TEST_F(ScRepoPathCollectorTestAPI, RepeatedRepos)
{
  std::string const & repoPath = TEST_REPEATED_REPOS;

  ScRepoPathCollector::Sources excludedSources, checkSources;
  EXPECT_TRUE(collector.IsRepoPathFile(repoPath));

  collector.ParseRepoPath(repoPath, excludedSources, checkSources);

  EXPECT_TRUE(excludedSources.empty());

  EXPECT_EQ(checkSources.size(), 1u);
  std::string const & directory = TEST_REPOS_KB;
  EXPECT_EQ(checkSources.count(directory), 1u);

  ScRepoPathCollector::Sources buildSources;
  collector.CollectBuildSources(repoPath, excludedSources, checkSources, buildSources);

  EXPECT_EQ(buildSources.size(), 13u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/templates/template1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/templates/template2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/templates/template3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/kb.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/rules.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/specification.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/example.gwf"), 1u);
}

TEST_F(ScRepoPathCollectorTestAPI, ExcludedRepos)
{
  std::string const & repoPath = TEST_EXCLUDED_REPOS;

  ScRepoPathCollector::Sources excludedSources, checkSources;
  EXPECT_TRUE(collector.IsRepoPathFile(repoPath));

  collector.ParseRepoPath(repoPath, excludedSources, checkSources);

  EXPECT_EQ(excludedSources.size(), 1u);
  std::string const & excludedDirectory = TEST_REPOS_KB "/templates";
  EXPECT_EQ(excludedSources.count(excludedDirectory), 1u);

  EXPECT_EQ(checkSources.size(), 1u);
  std::string const & directory = TEST_REPOS_KB;
  EXPECT_EQ(checkSources.count(directory), 1u);

  ScRepoPathCollector::Sources buildSources;
  collector.CollectBuildSources(repoPath, excludedSources, checkSources, buildSources);

  EXPECT_EQ(buildSources.size(), 10u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/kb.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/rules.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/specification.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/example.gwf"), 1u);
}

TEST_F(ScRepoPathCollectorTestAPI, NonTrimRepos)
{
  std::string const & repoPath = TEST_NON_TRIM_REPOS;

  ScRepoPathCollector::Sources excludedSources, checkSources;
  EXPECT_TRUE(collector.IsRepoPathFile(repoPath));

  collector.ParseRepoPath(repoPath, excludedSources, checkSources);

  EXPECT_EQ(excludedSources.size(), 1u);
  std::string const & excludedDirectory = TEST_REPOS_KB "/templates";
  EXPECT_EQ(excludedSources.count(excludedDirectory), 1u);

  EXPECT_EQ(checkSources.size(), 1u);
  std::string const & directory = TEST_REPOS_KB;
  EXPECT_EQ(checkSources.count(directory), 1u);

  ScRepoPathCollector::Sources buildSources;
  collector.CollectBuildSources(repoPath, excludedSources, checkSources, buildSources);

  EXPECT_EQ(buildSources.size(), 10u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/kb.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/rules.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/specification.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/example.gwf"), 1u);
}

TEST_F(ScRepoPathCollectorTestAPI, InvalidRepoPath)
{
  std::string const & repoPath = TEST_INVALID_REPO_PATH;

  ScRepoPathCollector::Sources excludedSources, checkSources;
  EXPECT_FALSE(collector.IsRepoPathFile(repoPath));

  EXPECT_THROW(collector.ParseRepoPath(repoPath, excludedSources, checkSources), utils::ExceptionInvalidState);

  EXPECT_TRUE(excludedSources.empty());
  EXPECT_TRUE(checkSources.empty());

  ScRepoPathCollector::Sources buildSources;
  collector.CollectBuildSources(repoPath, excludedSources, checkSources, buildSources);
  EXPECT_EQ(buildSources.size(), 0u);
}
