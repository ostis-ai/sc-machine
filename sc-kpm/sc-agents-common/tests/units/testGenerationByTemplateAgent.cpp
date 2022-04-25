/*
 * Copyright (c) 2022 Intelligent Semantic Systems LLC, All rights reserved.
 */

#include <gtest/gtest.h>

#include "agents/GenerationByTemplateAgent.hpp"
#include "keynodes/coreKeynodes.hpp"
#include "tests/keynodes/generationByTemplateKeynodes.hpp"
#include "utils/CommonUtils.hpp"

#include <tests/sc-memory/_test/dummy_file_interface.hpp>
#include "sc_agents_common_test.hpp"
#include "sc-memory/sc_wait.hpp"

namespace testGenerationByTemplateAgent
{
const int WAIT_TIME = 5000;
const std::string TEST_SCS_PATH = SC_AGENTS_COMMON_TEST_SRC_PATH "/GenerationByTemplate/";

void initializeClasses()
{
  scAgentsCommon::CoreKeynodes::InitGlobal();
  GenerationByTemplateKeynodes::InitGlobal();
}

std::string getScsStringFromFile(std::string fileName)
{
  std::ifstream fileStream(fileName);
  std::stringstream stringStream;
  stringStream << fileStream.rdbuf();
  return stringStream.str();
}

TEST_F(GenerationByTemplateTest, templateLinkNotFoundTest)
{
  ScMemoryContext & context = *m_ctx;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  std::string scsTestEnvironmentFilePath = TEST_SCS_PATH + "templateLinkNotFoundTestStructure.scs";
  std::string scsTestEnvironment = getScsStringFromFile(scsTestEnvironmentFilePath);
  EXPECT_TRUE(helper.GenerateBySCsText(scsTestEnvironment));

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action_node_without_link");
  EXPECT_TRUE(test_question_node.IsValid());

  ScAgentInit(true);
  initializeClasses();

  SC_AGENT_REGISTER(scAgentsCommon::GenerationByTemplateAgent)

  context.CreateEdge(
      ScType::EdgeAccessConstPosPerm, scAgentsCommon::CoreKeynodes::question_initiated, test_question_node);

  EXPECT_TRUE(ScWaitEvent<ScEventAddOutputEdge>(context, scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully)
                  .Wait(WAIT_TIME));

  SC_AGENT_UNREGISTER(scAgentsCommon::GenerationByTemplateAgent)
}

TEST_F(GenerationByTemplateTest, parameterNodeNotFoundTest)
{
  ScMemoryContext & context = *m_ctx;

  SCsHelper helperTemplate(*m_ctx, std::make_shared<DummyFileInterface>());
  std::string scsTestTemplateFilePath = TEST_SCS_PATH + "template.scs";
  std::string scsTestTemplate = getScsStringFromFile(scsTestTemplateFilePath);
  EXPECT_TRUE(helperTemplate.GenerateBySCsText(scsTestTemplate));

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  std::string scsTestEnvironmentFilePath = TEST_SCS_PATH + "parameterNodeNotFoundTestStructure.scs";
  std::string scsTestEnvironment = getScsStringFromFile(scsTestEnvironmentFilePath);
  EXPECT_TRUE(helper.GenerateBySCsText(scsTestEnvironment));

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action_node_without_parameter_node");
  EXPECT_TRUE(test_question_node.IsValid());

  ScAgentInit(true);
  initializeClasses();

  SC_AGENT_REGISTER(scAgentsCommon::GenerationByTemplateAgent)

  context.CreateEdge(
      ScType::EdgeAccessConstPosPerm, scAgentsCommon::CoreKeynodes::question_initiated, test_question_node);

  EXPECT_TRUE(ScWaitEvent<ScEventAddOutputEdge>(context, scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully)
                  .Wait(WAIT_TIME));

  SC_AGENT_UNREGISTER(scAgentsCommon::GenerationByTemplateAgent)
}

TEST_F(GenerationByTemplateTest, ruleStatementNotFoundTest)
{
  ScMemoryContext & context = *m_ctx;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  std::string scsTestEnvironmentFilePath = TEST_SCS_PATH + "ruleStatementNotFoundTestStructure.scs";
  std::string scsTestEnvironment = getScsStringFromFile(scsTestEnvironmentFilePath);
  EXPECT_TRUE(helper.GenerateBySCsText(scsTestEnvironment));

  ScAddr test_question_node = context.HelperFindBySystemIdtf("test_action_node_without_rule");
  EXPECT_TRUE(test_question_node.IsValid());

  ScAgentInit(true);
  initializeClasses();

  SC_AGENT_REGISTER(scAgentsCommon::GenerationByTemplateAgent)

  context.CreateEdge(
      ScType::EdgeAccessConstPosPerm, scAgentsCommon::CoreKeynodes::question_initiated, test_question_node);

  EXPECT_TRUE(ScWaitEvent<ScEventAddOutputEdge>(context, scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully)
                  .Wait(WAIT_TIME));

  SC_AGENT_UNREGISTER(scAgentsCommon::GenerationByTemplateAgent)
}

TEST_F(GenerationByTemplateTest, checkGeneretionByTemplateTest)
{
  ScMemoryContext & context = *m_ctx;

  SCsHelper helperTemplate(*m_ctx, std::make_shared<DummyFileInterface>());
  std::string scsTestTemplateFilePath = TEST_SCS_PATH + "template.scs";
  std::string scsTestTemplate = getScsStringFromFile(scsTestTemplateFilePath);
  EXPECT_TRUE(helperTemplate.GenerateBySCsText(scsTestTemplate));

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  std::string scsTestEnvironmentFilePath = TEST_SCS_PATH + "checkGenerationByTemplateTestStructure.scs";
  std::string scsTestEnvironment = getScsStringFromFile(scsTestEnvironmentFilePath);
  EXPECT_TRUE(helper.GenerateBySCsText(scsTestEnvironment));

  ScAddr test_question_node = context.HelperFindBySystemIdtf("success_test_action_node");
  EXPECT_TRUE(test_question_node.IsValid());

  string const USER_ALIAS = "_user";
  string const USER_LOGIN_ALIAS = "_user_login";

  ScAgentInit(true);
  initializeClasses();

  SC_AGENT_REGISTER(scAgentsCommon::GenerationByTemplateAgent)

  context.CreateEdge(
      ScType::EdgeAccessConstPosPerm, scAgentsCommon::CoreKeynodes::question_initiated, test_question_node);

  EXPECT_TRUE(ScWaitEvent<ScEventAddOutputEdge>(context, scAgentsCommon::CoreKeynodes::question_finished_successfully)
                  .Wait(WAIT_TIME));

  ScTemplate findUserTemplate;
  findUserTemplate.Triple(
      GenerationByTemplateKeynodes::test_concept, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> USER_ALIAS);
  findUserTemplate.TripleWithRelation(
      USER_ALIAS,
      ScType::EdgeDCommonVar,
      GenerationByTemplateKeynodes::test_lang,
      ScType::EdgeAccessVarPosPerm,
      GenerationByTemplateKeynodes::nrel_first);
  findUserTemplate.TripleWithRelation(
      USER_ALIAS,
      ScType::EdgeDCommonVar,
      ScType::LinkVar >> USER_LOGIN_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      GenerationByTemplateKeynodes::nrel_second);

  ScTemplateSearchResult searchResult;
  EXPECT_TRUE(context.HelperSearchTemplate(findUserTemplate, searchResult));
  EXPECT_TRUE(searchResult.Size() == 1);

  std::string typeArgumentNodeContent = utils::CommonUtils::getLinkContent(&context, searchResult[0][USER_LOGIN_ALIAS]);

  SC_AGENT_UNREGISTER(scAgentsCommon::GenerationByTemplateAgent)
}

}  // namespace testGenerationByTemplateAgent
