/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include "builder_test.hpp"

TEST_F(ScBuilderTest, Smoke)
{
  ScMemoryContext ctx;

  std::string const scsData =
      "base"
      " _<- test_element;"
      " _=> nrel_main_idtf::"
      "    _[]"
      "    (* _<- lang_en;; *);;";

  ScTemplate templ;
  ctx.BuildTemplate(templ, scsData);

  ScTemplateSearchResult res;
  EXPECT_TRUE(ctx.SearchByTemplate(templ, res));
}

TEST_F(ScBuilderTest, TemplateWithVarTriplesBig)
{
  ScAddr checkTemplateStructure = m_ctx->SearchElementBySystemIdentifier("test_template_big");

  ScTemplate checkTemplate;
  m_ctx->BuildTemplate(checkTemplate, checkTemplateStructure);

  size_t const constrCount = 50;
  for (size_t i = 0; i < constrCount; ++i)
  {
    ScTemplateGenResult genResult;
    m_ctx->GenerateByTemplate(checkTemplate, genResult);
  }

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(checkTemplate, searchResult));
  EXPECT_EQ(searchResult.Size(), constrCount);
}

TEST_F(ScBuilderTest, TemplateWithVarTriplesSmall)
{
  ScAddr checkTemplateStructure = m_ctx->SearchElementBySystemIdentifier("test_template_small");

  ScTemplate checkTemplate;
  m_ctx->BuildTemplate(checkTemplate, checkTemplateStructure);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(checkTemplate, searchResult));
}

TEST_F(ScBuilderTest, TemplateWithVarTriplesSmall2)
{
  ScAddr const & checkTemplateStructure = m_ctx->SearchElementBySystemIdentifier("test_template_small");

  ScTemplateParams params;
  ScAddr const & messageAddr = m_ctx->SearchElementBySystemIdentifier("message");
  params.Add("_message", messageAddr);

  ScAddr const & varAddr = m_ctx->SearchElementBySystemIdentifier("_message");

  ScTemplate checkTemplate;
  m_ctx->BuildTemplate(checkTemplate, checkTemplateStructure, params);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(checkTemplate, searchResult));
  EXPECT_EQ(searchResult[0]["_message"], messageAddr);
  EXPECT_EQ(searchResult[0][varAddr], messageAddr);
}

TEST_F(ScBuilderTest, SearchTemplateWithVarAddrSubstituteInParams)
{
  ScAddr const & checkTemplateStructure = m_ctx->SearchElementBySystemIdentifier("test_template_small");

  ScTemplateParams params;
  ScAddr const & messageAddr = m_ctx->SearchElementBySystemIdentifier("message");
  ScAddr const & varAddr = m_ctx->SearchElementBySystemIdentifier("_message");
  params.Add(varAddr, messageAddr);

  ScTemplate checkTemplate;
  m_ctx->BuildTemplate(checkTemplate, checkTemplateStructure, params);
  EXPECT_FALSE(checkTemplate.HasReplacement(varAddr));

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(checkTemplate, searchResult));
  EXPECT_THROW(searchResult[0]["_message"], utils::ExceptionInvalidParams);
  EXPECT_THROW(searchResult[0][varAddr], utils::ExceptionInvalidParams);
}

TEST_F(ScBuilderTest, TemplateWithVarTriplesSmall3)
{
  ScAddr const & checkTemplateStructure = m_ctx->SearchElementBySystemIdentifier("test_template_small");

  ScTemplateParams params;
  ScAddr const & messageAddr = m_ctx->SearchElementBySystemIdentifier("message");
  params.Add("_message", messageAddr);

  ScAddr const & varAddr = m_ctx->SearchElementBySystemIdentifier("_message");

  ScTemplate checkTemplate;
  m_ctx->BuildTemplate(checkTemplate, checkTemplateStructure);

  ScTemplateGenResult genResult;
  m_ctx->GenerateByTemplate(checkTemplate, genResult, params);
  EXPECT_EQ(genResult["_message"], messageAddr);
  EXPECT_EQ(genResult[varAddr], messageAddr);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(m_ctx->SearchByTemplate(checkTemplate, searchResult));
  EXPECT_EQ(searchResult.Size(), 2u);
  EXPECT_EQ(searchResult[0]["_message"], messageAddr);
  EXPECT_EQ(searchResult[1]["_message"], messageAddr);
  EXPECT_EQ(searchResult[0][varAddr], messageAddr);
  EXPECT_EQ(searchResult[1][varAddr], messageAddr);

  SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_BEGIN
  auto const & replacements = searchResult.GetReplacements();
  SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_END
  EXPECT_EQ(replacements.count("_message"), 1u);
  EXPECT_EQ(replacements.count(std::to_string(varAddr.Hash())), 1u);
}
