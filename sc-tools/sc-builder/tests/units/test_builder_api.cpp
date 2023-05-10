#include <gtest/gtest.h>

#include "builder_test_api.hpp"

#define TEST_SIMPLE_REPO SC_BUILDER_TEST_REPOS "/simple_repo.path"
#define TEST_REPEATED_REPOS SC_BUILDER_TEST_REPOS "/repeated_repos.path"
#define TEST_EXCLUDED_REPOS SC_BUILDER_TEST_REPOS "/excluded_repos.path"
#define TEST_NON_TRIM_REPOS SC_BUILDER_TEST_REPOS "/non_trim_repos.path"
#define TEST_INVALID_REPO_PATH SC_BUILDER_TEST_REPOS "/invalid_repo_path"

TEST_F(ScBuilderTestAPI, Sources)
{
  std::string const & directory = TEST_REPOS_KB;

  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/child_repos/child_kb/samples/sample1.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/child_repos/child_kb/samples/sample2.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/child_repos/child_kb/samples/sample3.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/samples/sample1.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/samples/sample2.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/samples/sample3.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/templates/template1.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/templates/template2.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/templates/template3.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/kb.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/rules.scs"));
  EXPECT_TRUE(builder.TestIsSourceFile(directory + "/specification.scs"));
  EXPECT_FALSE(builder.TestIsSourceFile(directory + "/example.gwf"));
  EXPECT_FALSE(builder.TestIsSourceFile(directory + "/invalid_file"));
}

TEST_F(ScBuilderTestAPI, SimpleRepo)
{
  std::string const & repoPath = TEST_SIMPLE_REPO;

  Builder::Sources excludedSources, checkSources;
  EXPECT_TRUE(builder.TestIsRepoPathFile(repoPath));

  builder.TestParseRepoPath(repoPath, excludedSources, checkSources);

  EXPECT_TRUE(excludedSources.empty());

  EXPECT_EQ(checkSources.size(), 1u);
  std::string const & directory = TEST_REPOS_KB;
  EXPECT_EQ(checkSources.count(directory), 1u);

  Builder::Sources buildSources;
  builder.TestCollectBuildSources(repoPath, excludedSources, checkSources, buildSources);

  EXPECT_EQ(buildSources.size(), 12u);
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
}

TEST_F(ScBuilderTestAPI, RepeatedRepos)
{
  std::string const & repoPath = TEST_REPEATED_REPOS;

  Builder::Sources excludedSources, checkSources;
  EXPECT_TRUE(builder.TestIsRepoPathFile(repoPath));

  builder.TestParseRepoPath(repoPath, excludedSources, checkSources);

  EXPECT_TRUE(excludedSources.empty());

  EXPECT_EQ(checkSources.size(), 1u);
  std::string const & directory = TEST_REPOS_KB;
  EXPECT_EQ(checkSources.count(directory), 1u);

  Builder::Sources buildSources;
  builder.TestCollectBuildSources(repoPath, excludedSources, checkSources, buildSources);

  EXPECT_EQ(buildSources.size(), 12u);
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
}

TEST_F(ScBuilderTestAPI, ExcludedRepos)
{
  std::string const & repoPath = TEST_EXCLUDED_REPOS;

  Builder::Sources excludedSources, checkSources;
  EXPECT_TRUE(builder.TestIsRepoPathFile(repoPath));

  builder.TestParseRepoPath(repoPath, excludedSources, checkSources);

  EXPECT_EQ(excludedSources.size(), 1u);
  std::string const & excludedDirectory = TEST_REPOS_KB "/templates";
  EXPECT_EQ(excludedSources.count(excludedDirectory), 1u);

  EXPECT_EQ(checkSources.size(), 1u);
  std::string const & directory = TEST_REPOS_KB;
  EXPECT_EQ(checkSources.count(directory), 1u);

  Builder::Sources buildSources;
  builder.TestCollectBuildSources(repoPath, excludedSources, checkSources, buildSources);

  EXPECT_EQ(buildSources.size(), 9u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/kb.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/rules.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/specification.scs"), 1u);
}

TEST_F(ScBuilderTestAPI, NonTrimRepos)
{
  std::string const & repoPath = TEST_NON_TRIM_REPOS;

  Builder::Sources excludedSources, checkSources;
  EXPECT_TRUE(builder.TestIsRepoPathFile(repoPath));

  builder.TestParseRepoPath(repoPath, excludedSources, checkSources);

  EXPECT_EQ(excludedSources.size(), 1u);
  std::string const & excludedDirectory = TEST_REPOS_KB "/templates";
  EXPECT_EQ(excludedSources.count(excludedDirectory), 1u);

  EXPECT_EQ(checkSources.size(), 1u);
  std::string const & directory = TEST_REPOS_KB;
  EXPECT_EQ(checkSources.count(directory), 1u);

  Builder::Sources buildSources;
  builder.TestCollectBuildSources(repoPath, excludedSources, checkSources, buildSources);

  EXPECT_EQ(buildSources.size(), 9u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/child_repos/child_kb/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample1.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample2.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/samples/sample3.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/kb.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/rules.scs"), 1u);
  EXPECT_EQ(buildSources.count(directory + "/specification.scs"), 1u);
}

TEST_F(ScBuilderTestAPI, InvalidRepoPath)
{
  std::string const & repoPath = TEST_INVALID_REPO_PATH;

  Builder::Sources excludedSources, checkSources;
  EXPECT_FALSE(builder.TestIsRepoPathFile(repoPath));

  EXPECT_THROW(builder.TestParseRepoPath(repoPath, excludedSources, checkSources), utils::ExceptionInvalidState);

  EXPECT_TRUE(excludedSources.empty());
  EXPECT_TRUE(checkSources.empty());

  Builder::Sources buildSources;
  builder.TestCollectBuildSources(repoPath, excludedSources, checkSources, buildSources);
  EXPECT_EQ(buildSources.size(), 0u);
}
