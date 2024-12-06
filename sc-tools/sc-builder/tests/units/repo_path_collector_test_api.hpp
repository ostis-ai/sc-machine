/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <gtest/gtest.h>

#include "sc_repo_path_collector.hpp"

class ScRepoPathCollectorTestAPI : public testing::Test
{
public:
  static inline std::string const & SC_BUILDER_TEST_REPOS = "../repos";
  static inline std::string const & SC_BUILDER_TEST_REPOS_KB = SC_BUILDER_TEST_REPOS + "/kb";
  static inline std::string const & SC_BUILDER_TEST_SIMPLE_REPO_PATH = SC_BUILDER_TEST_REPOS + "/simple_repo.path";
  static inline std::string const & SC_BUILDER_TEST_REPEATED_REPOS = SC_BUILDER_TEST_REPOS + "/repeated_repos.path";
  static inline std::string const & SC_BUILDER_TEST_EXCLUDED_REPOS = SC_BUILDER_TEST_REPOS + "/excluded_repos.path";
  static inline std::string const & SC_BUILDER_TEST_NON_TRIM_REPOS = SC_BUILDER_TEST_REPOS + "/non_trim_repos.path";
  static inline std::string const & SC_BUILDER_TEST_INVALID_REPO_PATH =
      SC_BUILDER_TEST_REPOS + "/invalid_repo_path.path";

protected:
  ScRepoPathCollector collector;

  void SetUp() override
  {
    collector = ScRepoPathCollector();
  }

  void TearDown() override {}
};
