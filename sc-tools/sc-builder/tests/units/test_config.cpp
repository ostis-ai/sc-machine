/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder_test.hpp"

#include <sc-builder/builder.hpp>

#include <sc-config/sc_options.hpp>
#include <sc-config/sc_config.hpp>
#include <sc-config/sc_memory_config.hpp>

#include "sc_builder_runner.hpp"

TEST(ScBuilder, Run)
{
  sc_uint32 const argsNumber = 8;
  sc_char const * args[argsNumber] = {
      "sc-builder", "-c", ScBuilderTest::SC_BUILDER_INI.c_str(), "-i", ScBuilderTest::SC_BUILDER_REPO_PATH.c_str(), "-o", ScBuilderTest::SC_BUILDER_KB_BIN.c_str(), "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScBuilder, RunWithOptionsFromBuilderGroup)
{
  std::string const & configPath = ScBuilderTest::SC_BUILDER_CONFIGS + "/used-builder-group.ini";

  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-builder", "-c", configPath.c_str(), "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScBuilder, RunWithOptionsFromBuilderGroupAndWithOptionsFromCommandLine)
{
  std::string const & configPath = ScBuilderTest::SC_BUILDER_CONFIGS + "/unused-builder-group.ini";

  sc_uint32 const argsNumber = 8;
  sc_char const * args[argsNumber] = {
      "sc-builder",
      "-c",
      configPath.c_str(),
      "-i",
      ScBuilderTest::SC_BUILDER_REPO_PATH.c_str(),
      "-o",
      ScBuilderTest::SC_BUILDER_KB_BIN.c_str(),
      "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScBuilder, RunWithoutBuilderGroupAndWithOptionsFromCommandLine)
{
  std::string const & configPath = ScBuilderTest::SC_BUILDER_CONFIGS + "/without-builder-group.ini";

  sc_uint32 const argsNumber = 8;
  sc_char const * args[argsNumber] = {
      "sc-builder",
      "-c",
      configPath.c_str(),
      "-i",
      ScBuilderTest::SC_BUILDER_REPO_PATH.c_str(),
      "-o",
      ScBuilderTest::SC_BUILDER_KB_BIN.c_str(),
      "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScBuilder, RunWithoutBuilderGroupAndWithoutInputOption)
{
  std::string const & configPath = ScBuilderTest::SC_BUILDER_CONFIGS + "/without-builder-group.ini";

  sc_uint32 const argsNumber = 6;
  sc_char const * args[argsNumber] = {
      "sc-builder", "-c", configPath.c_str(), "-o", ScBuilderTest::SC_BUILDER_KB_BIN.c_str(), "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_FAILURE);
}

TEST(ScBuilder, RunWithoutBuilderGroupAndWithoutOutputOption)
{
  std::string const & configPath = ScBuilderTest::SC_BUILDER_CONFIGS + "/without-builder-group.ini";

  sc_uint32 const argsNumber = 6;
  sc_char const * args[argsNumber] = {
      "sc-builder", "-c", configPath.c_str(), "-i", ScBuilderTest::SC_BUILDER_KB.c_str(), "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_FAILURE);
}

TEST(ScBuilder, RunWithDeprecatedInputPathOption)
{
  std::string const & configPath = ScBuilderTest::SC_BUILDER_CONFIGS + "/deprecated-input-path.ini";

  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-builder", "-c", configPath.c_str(), "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScBuilder, RunWithOutputPathAsMemoryStorage)
{
  std::string const & configPath = ScBuilderTest::SC_BUILDER_CONFIGS + "/output-path-as-storage.ini";

  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-builder", "-c", configPath.c_str(), "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScBuilder, RunWithoutConfig)
{
  sc_uint32 const argsNumber = 6;
  sc_char const * args[argsNumber] = {"sc-builder", "-i", ScBuilderTest::SC_BUILDER_REPO_PATH.c_str(), "-o", ScBuilderTest::SC_BUILDER_KB_BIN.c_str(), "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScBuilder, RunWithoutInput)
{
  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-builder", "-o", ScBuilderTest::SC_BUILDER_KB_BIN.c_str(), "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_FAILURE);
}

TEST(ScBuilder, RunWithoutOutput)
{
  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-builder", "-i", ScBuilderTest::SC_BUILDER_REPO_PATH.c_str(), "--clear"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_FAILURE);
}

TEST(ScBuilder, InvalidRun)
{
  sc_uint32 const argsNumber = 1;
  sc_char const * args[argsNumber] = {"sc-builder"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_FAILURE);
}

TEST(ScBuilder, PrintHelp)
{
  sc_uint32 const argsNumber = 2;
  sc_char const * args[argsNumber] = {"sc-builder", "--help"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScBuilder, PrintVersion)
{
  sc_uint32 const argsNumber = 2;
  sc_char const * args[argsNumber] = {"sc-builder", "--version"};
  EXPECT_EQ(RunBuilder(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScBuilder, RunStopBuilder)
{
  ScOptions options{1, nullptr};

  BuilderParams params;
  params.m_inputPath = ScBuilderTest::SC_BUILDER_REPO_PATH;
  params.m_outputPath = ScBuilderTest::SC_BUILDER_KB_BIN;

  std::string config = ScBuilderTest::SC_BUILDER_INI;

  ScParams memoryParams{options, {}};
  memoryParams.Insert({"storage", ScBuilderTest::SC_BUILDER_KB_BIN});

  ScConfig configFile{config, {"storage"}};
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
    scParams.Insert({key, value});
  }
  sc_memory_params const scMemoryParams = memoryConfig.GetParams();

  EXPECT_EQ(scParams.Get<sc_uint32>("max_loaded_segments"), scMemoryParams.max_loaded_segments);
  EXPECT_EQ(scParams.Get<sc_uint32>("dump_memory_period"), scMemoryParams.dump_memory_period);
  EXPECT_EQ(scParams.Get<sc_uint32>("dump_memory_statistics_period"), scMemoryParams.dump_memory_statistics_period);
  EXPECT_EQ(scParams.Get<std::string>("log_type"), scMemoryParams.log_type);
  EXPECT_EQ(scParams.Get<std::string>("log_file"), scMemoryParams.log_file);
  EXPECT_EQ(scParams.Get<std::string>("log_level"), scMemoryParams.log_level);
  EXPECT_EQ(
      scParams.Get<std::string>("init_memory_generated_upload") == "true" ? true : false,
      scMemoryParams.init_memory_generated_upload);
  EXPECT_EQ(
      scParams.Get<std::string>("init_memory_generated_structure"), scMemoryParams.init_memory_generated_structure);
}

TEST(ScBuilder, InvalidConfig)
{
  ScOptions options{1, nullptr};

  BuilderParams params;

  ScParams memoryParams{options, {}};
  ScConfig configFile{"", {}};
  ScMemoryConfig memoryConfig{configFile, memoryParams};

  Builder builder;
  EXPECT_THROW(builder.Run(params, memoryConfig.GetParams()), utils::ScException);
}

TEST(ScBuilder, BuilderConfig)
{
  ScOptions options{1, nullptr};

  BuilderParams builderParams;
  builderParams.m_inputPath = ScBuilderTest::SC_BUILDER_REPO_PATH;
  builderParams.m_outputPath = ScBuilderTest::SC_BUILDER_KB_BIN;

  std::string config = ScBuilderTest::SC_BUILDER_INI;
  ScConfig configFile{config, {"storage"}};

  ScParams memoryParams{options, {}};
  memoryParams.Insert({"storage", ScBuilderTest::SC_BUILDER_KB_BIN});
  memoryParams.Insert({"clear", {}});
  ScMemoryConfig memoryConfig{configFile, memoryParams};

  sc_memory_params const memory_params = memoryConfig.GetParams();
  builderParams.m_resultStructureUpload = memory_params.init_memory_generated_upload;
  builderParams.m_resultStructureSystemIdtf =
      builderParams.m_resultStructureUpload ? std::string(memory_params.init_memory_generated_structure) : "";

  Builder builder;
  EXPECT_TRUE(builder.Run(builderParams, memoryConfig.GetParams()));

  std::string builderGroupName = "sc-builder";
  std::string memoryGroupName = "sc-memory";

  ScParams scParams{options, {}};
  EXPECT_TRUE(configFile.IsValid());
  ScConfigGroup group = configFile[builderGroupName];
  for (std::string const & key : *group)
  {
    std::string const & value = group[key];
    scParams.Insert({key, value});
  }

  group = configFile[memoryGroupName];
  for (std::string const & key : *group)
  {
    std::string const & value = group[key];
    scParams.Insert({key, value});
  }

  EXPECT_EQ(
      scParams.Get<std::string>("init_memory_generated_upload") == "true" ? true : false,
      builderParams.m_resultStructureUpload);
  EXPECT_EQ(scParams.Get<std::string>("init_memory_generated_structure"), builderParams.m_resultStructureSystemIdtf);

  sc_memory_params newMemoryParams = memoryConfig.GetParams();
  newMemoryParams.clear = false;
  ScMemory::Initialize(newMemoryParams);

  auto * context = new ScMemoryContext();

  ScSystemIdentifierQuintuple quintuple;
  context->SearchElementBySystemIdentifier(builderParams.m_resultStructureSystemIdtf, quintuple);
  ScAddr const & resultStructure = quintuple.addr1;
  EXPECT_TRUE(resultStructure.IsValid());

  auto const & CheckInStructure = [&context, &resultStructure](ScAddr const & addr)
  {
    EXPECT_TRUE(context->CheckConnector(resultStructure, addr, ScType::ConstPermPosArc));
  };

  CheckInStructure(quintuple.addr2);
  CheckInStructure(quintuple.addr3);
  CheckInStructure(quintuple.addr4);
  CheckInStructure(quintuple.addr5);

  context->Destroy();
  delete context;
  ScMemory::Shutdown(SC_FALSE);
}
