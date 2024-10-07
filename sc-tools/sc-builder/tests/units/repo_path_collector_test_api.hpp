/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <gtest/gtest.h>

#include "sc_repo_path_collector.hpp"

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

  void TearDown() override {}
};
