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
  memoryParams.insert({"clear", "true"});

  ScConfig configFile{config, {"repo_path"}};
  std::string memoryGroupName = "sc-memory";

  ScMemoryConfig memoryConfig{configFile, std::move(memoryParams)};

  Builder builder;
  EXPECT_TRUE(builder.Run(params, memoryConfig.GetParams()));

  ScParams scParams{options, {}};
  if (configFile.IsValid())
  {
    ScConfigGroup group = configFile[memoryGroupName];
    for (auto const & key : *group)
    {
      std::string const & value = group[key];
      std::stringstream stream;

      scParams.insert({key, value});
    }
    sc_memory_params const scMemoryParams = memoryConfig.GetParams();

    EXPECT_EQ(std::stoi(scParams.at("max_loaded_segments")), scMemoryParams.max_loaded_segments);
    EXPECT_EQ(std::stoi(scParams.at("max_threads")), scMemoryParams.max_threads);
    EXPECT_EQ(std::stoi(scParams.at("save_period")), scMemoryParams.save_period);
    EXPECT_EQ(std::stoi(scParams.at("update_period")), scMemoryParams.update_period);
    EXPECT_EQ(scParams.at("log_type"), scMemoryParams.log_type);
    EXPECT_EQ(scParams.at("log_file"), scMemoryParams.log_file);
    EXPECT_EQ(scParams.at("log_level"), scMemoryParams.log_level);
  }
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
  memoryParams.insert({"clear", "false"});

  ScConfig configFile{config, {"repo_path"}};

  ScBuilderConfig builderConfig{configFile, std::move(params)};
  ScMemoryConfig memoryConfig{configFile, std::move(memoryParams)};

  Builder builder;
  BuilderParams builderParams = builderConfig.GetParams();
  EXPECT_TRUE(builder.Run(builderParams, memoryConfig.GetParams()));

  std::string builderGroupName = "sc-builder";

  ScParams scParams{options, {}};
  if (configFile.IsValid())
  {
    ScConfigGroup group = configFile[builderGroupName];
    for (auto const & key : *group)
    {
      std::string const & value = group[key];
      std::stringstream stream;

      scParams.insert({key, value});
    }

    EXPECT_EQ(scParams.at("result_structure_upload") == "true" ? true : false, builderParams.m_resultStructureUpload);
    EXPECT_EQ(scParams.at("result_structure_system_idtf"), builderParams.m_resultStructureSystemIdtf);
  }
}

TEST_F(ScBuilderTest, Smoke)
{
  ScMemoryContext ctx("Builder_Base");

  std::string const scsData =
      "base"
      " _<- test_element;"
      " _=> nrel_main_idtf::"
      "    _[]"
      "    (* _<- lang_en;; *);;";

  ScTemplate templ;
  EXPECT_TRUE(ctx.HelperBuildTemplate(templ, scsData));

  ScTemplateSearchResult res;
  EXPECT_TRUE(ctx.HelperSearchTemplate(templ, res));
}

TEST_F(ScBuilderTest, TemplateWithVarTriplesBig)
{
  ScAddr checkTemplateStructure = m_ctx->HelperFindBySystemIdtf("test_template_big");

  ScTemplate checkTemplate;
  EXPECT_TRUE(m_ctx->HelperBuildTemplate(checkTemplate, checkTemplateStructure));

  ScTemplateGenResult genResult;
  EXPECT_TRUE(m_ctx->HelperGenTemplate(checkTemplate, genResult));

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(checkTemplate, searchResult));
}

TEST_F(ScBuilderTest, TemplateWithVarTriplesSmall)
{
  ScAddr checkTemplateStructure = m_ctx->HelperFindBySystemIdtf("test_template_small");

  ScTemplate checkTemplate;
  EXPECT_TRUE(m_ctx->HelperBuildTemplate(checkTemplate, checkTemplateStructure));

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->HelperSearchTemplate(checkTemplate, searchResult));
}
