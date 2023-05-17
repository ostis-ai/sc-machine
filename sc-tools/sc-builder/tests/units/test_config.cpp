/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <gtest/gtest.h>

#include "builder_test.hpp"

#include "../../src/builder.hpp"

#include "sc_options.hpp"
#include "sc_memory_config.hpp"
#include "sc_builder_config.hpp"

TEST(ScBuilder, RunStop)
{
  ScOptions options{1, nullptr};

  BuilderParams params;
  params.m_inputPath = SC_BUILDER_KB;
  params.m_outputPath = SC_BUILDER_REPO_PATH;
  params.m_autoFormatInfo = SC_TRUE;

  std::string config = SC_BUILDER_INI;

  ScParams memoryParams{options, {}};
  memoryParams.insert({"repo_path", SC_BUILDER_REPO_PATH});

  ScConfig configFile{config, {"repo_path"}};
  std::string memoryGroupName = "sc-memory";

  ScMemoryConfig memoryConfig{configFile, memoryParams};

  Builder builder;
  EXPECT_TRUE(builder.Run(params, memoryConfig.GetParams()));

  ScParams scParams{options, {}};
  EXPECT_TRUE(configFile.IsValid());
  ScConfigGroup group = configFile[memoryGroupName];
  for (std::string const & key : *group)
  {
    std::string const & value = group[key];
    scParams.insert({key, value});
  }
  sc_memory_params const scMemoryParams = memoryConfig.GetParams();

  EXPECT_EQ((sc_uint32)std::stoi(scParams.at("max_loaded_segments")), scMemoryParams.max_loaded_segments);
  EXPECT_EQ((sc_uint8)std::stoi(scParams.at("max_threads")), scMemoryParams.max_threads);
  EXPECT_EQ((sc_uint32)std::stoi(scParams.at("save_period")), scMemoryParams.save_period);
  EXPECT_EQ((sc_uint32)std::stoi(scParams.at("update_period")), scMemoryParams.update_period);
  EXPECT_EQ(scParams.at("log_type"), scMemoryParams.log_type);
  EXPECT_EQ(scParams.at("log_file"), scMemoryParams.log_file);
  EXPECT_EQ(scParams.at("log_level"), scMemoryParams.log_level);
  EXPECT_EQ(
      scParams.at("init_memory_generated_upload") == "true" ? true : false, scMemoryParams.init_memory_generated_upload);
  EXPECT_EQ(scParams.at("init_memory_generated_structure"), scMemoryParams.init_memory_generated_structure);
}

TEST(ScBuilder, BuilderConfig)
{
  ScOptions options{1, nullptr};

  BuilderParams params;
  params.m_inputPath = SC_BUILDER_KB;
  params.m_outputPath = SC_BUILDER_REPO_PATH;
  params.m_autoFormatInfo = SC_TRUE;

  std::string config = SC_BUILDER_INI;

  ScParams memoryParams{options, {}};
  memoryParams.insert({"repo_path", SC_BUILDER_REPO_PATH});
  memoryParams.insert({"clear", {}});

  ScConfig configFile{config, {"repo_path"}};

  ScMemoryConfig memoryConfig{configFile, memoryParams};

  params.m_resultStructureUpload = memoryConfig.GetParams().init_memory_generated_upload;
  params.m_resultStructureSystemIdtf =
      params.m_resultStructureUpload ? std::string(memoryConfig.GetParams().init_memory_generated_structure) : "";

  ScBuilderConfig builderConfig{configFile, std::move(params)};

  Builder builder;
  BuilderParams builderParams = builderConfig.GetParams();
  EXPECT_TRUE(builder.Run(builderParams, memoryConfig.GetParams()));

  std::string builderGroupName = "sc-builder";
  std::string memoryGroupName = "sc-memory";

  ScParams scParams{options, {}};
  EXPECT_TRUE(configFile.IsValid());
  ScConfigGroup group = configFile[builderGroupName];
  for (std::string const & key : *group)
  {
    std::string const & value = group[key];
    scParams.insert({key, value});
  }

  group = configFile[memoryGroupName];
  for (std::string const & key : *group)
  {
    std::string const & value = group[key];
    scParams.insert({key, value});
  }

  EXPECT_EQ(
      scParams.at("init_memory_generated_upload") == "true" ? true : false, builderParams.m_resultStructureUpload);
  EXPECT_EQ(scParams.at("init_memory_generated_structure"), builderParams.m_resultStructureSystemIdtf);

  sc_memory_params newMemoryParams = memoryConfig.GetParams();
  newMemoryParams.clear = false;
  ScMemory::Initialize(newMemoryParams);

  auto * context = new ScMemoryContext("check_context");

  ScSystemIdentifierFiver fiver;
  context->HelperFindBySystemIdtf(builderParams.m_resultStructureSystemIdtf, fiver);
  ScAddr const & resultStructure = fiver.addr1;
  EXPECT_TRUE(resultStructure.IsValid());

  auto const & CheckInStructure = [&context, &resultStructure](ScAddr const & addr) {
    EXPECT_TRUE(context->HelperCheckEdge(resultStructure, addr, ScType::EdgeAccessConstPosPerm));
  };

  CheckInStructure(fiver.addr2);
  CheckInStructure(fiver.addr3);
  CheckInStructure(fiver.addr4);
  CheckInStructure(fiver.addr5);

  context->Destroy();
  delete context;
  ScMemory::Shutdown(false);
}
