#pragma once

#include <gtest/gtest.h>

#include "../../src/sc_repo_path_collector.hpp"

#include "test_defines.hpp"

#define TEST_REPOS_KB SC_BUILDER_TEST_REPOS "/kb"

class ScRepoPathCollectorTestAPI : public testing::Test
{
protected:
  ScRepoPathCollector collector;

  void SetUp() override
  {
    collector = ScRepoPathCollector();
  }

  void TearDown() override
  {
  }
};
