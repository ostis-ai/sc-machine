#include <gtest/gtest.h>

#include "builder_test.hpp"

#include "../../src/builder.hpp"

#include "sc_options.hpp"
#include "sc_memory_config.hpp"

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

  ScMemoryConfig memoryConfig{config, {"repo_path"}, std::move(memoryParams)};

  Builder builder;
  EXPECT_TRUE(builder.Run(params, memoryConfig.GetParams()));
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
