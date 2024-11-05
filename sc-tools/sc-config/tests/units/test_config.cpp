/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include "test_defines.hpp"

#include <sc-config/sc_config.hpp>
#include <sc-config/sc_memory_config.hpp>

TEST(ScConfig, ValidConfig)
{
  ScConfig config{SC_MACHINE_INI};

  ScMemoryConfig memoryConfig{config, {}};

  sc_memory_params const params = memoryConfig.GetParams();
  EXPECT_EQ(params.max_loaded_segments, 1000u);
  EXPECT_EQ(params.dump_memory, SC_TRUE);
  EXPECT_EQ(params.dump_memory_period, 4u);
  EXPECT_EQ(params.dump_memory_statistics, SC_TRUE);
  EXPECT_EQ(params.dump_memory_statistics_period, 4u);
  EXPECT_EQ(std::string(params.log_type), "Console");
  EXPECT_EQ(std::string(params.log_file), "");
  EXPECT_EQ(std::string(params.log_level), "Debug");
  EXPECT_EQ(std::string(params.storage), "sc-machine-test-repo");
  EXPECT_EQ(params.extensions_directories_count, 1u);
  EXPECT_EQ(std::string(params.extensions_directories[0]), "bin/extensions");
}

TEST(ScConfig, RemoveSavePeriod)
{
  ScConfig config{SC_CONFIGS "/removed-save-period.ini"};

  ScMemoryConfig memoryConfig{config, {}};

  EXPECT_THROW(memoryConfig.GetParams(), utils::ExceptionInvalidParams);
}

TEST(ScConfig, RemoveUpdatePeriod)
{
  ScConfig config{SC_CONFIGS "/removed-update-period.ini"};

  ScMemoryConfig memoryConfig{config, {}};

  EXPECT_THROW(memoryConfig.GetParams(), utils::ExceptionInvalidParams);
}

TEST(ScConfig, DeprecatedRepoPath)
{
  ScConfig config{SC_CONFIGS "/deprecated-repo-path.ini"};

  ScMemoryConfig memoryConfig{config, {}};

  sc_memory_params const params = memoryConfig.GetParams();
  EXPECT_EQ(params.max_loaded_segments, 1000u);
  EXPECT_EQ(params.dump_memory, SC_TRUE);
  EXPECT_EQ(params.dump_memory_period, 4u);
  EXPECT_EQ(params.dump_memory_statistics, SC_TRUE);
  EXPECT_EQ(params.dump_memory_statistics_period, 4u);
  EXPECT_EQ(std::string(params.log_type), "Console");
  EXPECT_EQ(std::string(params.log_file), "");
  EXPECT_EQ(std::string(params.log_level), "Debug");
  EXPECT_EQ(std::string(params.storage), "sc-machine-test-repo");
  EXPECT_EQ(params.extensions_directories_count, 1u);
  EXPECT_EQ(std::string(params.extensions_directories[0]), "bin/extensions");
}

TEST(ScConfig, DeprecatedExtensionsPath)
{
  ScConfig config{SC_CONFIGS "/deprecated-extensions-path.ini"};

  ScMemoryConfig memoryConfig{config, {}};

  sc_memory_params const params = memoryConfig.GetParams();
  EXPECT_EQ(params.max_loaded_segments, 1000u);
  EXPECT_EQ(params.dump_memory, SC_TRUE);
  EXPECT_EQ(params.dump_memory_period, 4u);
  EXPECT_EQ(params.dump_memory_statistics, SC_TRUE);
  EXPECT_EQ(params.dump_memory_statistics_period, 4u);
  EXPECT_EQ(std::string(params.log_type), "Console");
  EXPECT_EQ(std::string(params.log_file), "");
  EXPECT_EQ(std::string(params.log_level), "Debug");
  EXPECT_EQ(std::string(params.storage), "sc-machine-test-repo");
  EXPECT_EQ(params.extensions_directories_count, 1u);
  EXPECT_EQ(std::string(params.extensions_directories[0]), "bin/extensions");
}

TEST(ScConfig, Multiextensions)
{
  ScConfig config{SC_CONFIGS "/multiextensions.ini"};

  ScMemoryConfig memoryConfig{config, {}};

  sc_memory_params const params = memoryConfig.GetParams();
  EXPECT_EQ(params.max_loaded_segments, 1000u);
  EXPECT_EQ(params.dump_memory, SC_TRUE);
  EXPECT_EQ(params.dump_memory_period, 4u);
  EXPECT_EQ(params.dump_memory_statistics, SC_TRUE);
  EXPECT_EQ(params.dump_memory_statistics_period, 4u);
  EXPECT_EQ(std::string(params.log_type), "Console");
  EXPECT_EQ(std::string(params.log_file), "");
  EXPECT_EQ(std::string(params.log_level), "Debug");
  EXPECT_EQ(std::string(params.storage), "sc-machine-test-repo");
  EXPECT_EQ(params.extensions_directories_count, 3u);
  EXPECT_EQ(std::string(params.extensions_directories[0]), "bin/extensions_1");
  EXPECT_EQ(std::string(params.extensions_directories[1]), "bin/extensions_2");
  EXPECT_EQ(std::string(params.extensions_directories[2]), "bin/extensions_3");
}

TEST(ScConfig, EmptyMultiextensions)
{
  ScConfig config{SC_CONFIGS "/empty-multiextensions.ini"};

  ScMemoryConfig memoryConfig{config, {}};

  sc_memory_params const params = memoryConfig.GetParams();
  EXPECT_EQ(params.max_loaded_segments, 1000u);
  EXPECT_EQ(params.dump_memory, SC_TRUE);
  EXPECT_EQ(params.dump_memory_period, 4u);
  EXPECT_EQ(params.dump_memory_statistics, SC_TRUE);
  EXPECT_EQ(params.dump_memory_statistics_period, 4u);
  EXPECT_EQ(std::string(params.log_type), "Console");
  EXPECT_EQ(std::string(params.log_file), "");
  EXPECT_EQ(std::string(params.log_level), "Debug");
  EXPECT_EQ(std::string(params.storage), "sc-machine-test-repo");
  EXPECT_EQ(params.extensions_directories_count, 0u);
  EXPECT_EQ(params.extensions_directories, nullptr);
}
