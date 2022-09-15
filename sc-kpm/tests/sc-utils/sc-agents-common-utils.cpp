#include <gtest/gtest.h>

#include "sc_test.hpp"
#include "dummy_file_interface.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"

#include "tests/test-agents/FinishActionTestAgent.hpp"

TEST_F(ScMemoryTest, getAnyByOutRelation)
{
  std::string const data =
      "dialog"
      "  <- concept_dialog;"
      "  -> rrel_last: message; "
      "  -> rrel_1: message;; ";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const dialog = m_ctx->HelperFindBySystemIdtf("dialog");
  EXPECT_TRUE(dialog.IsValid());
  ScAddr const relation = m_ctx->HelperFindBySystemIdtf("rrel_1");
  EXPECT_TRUE(relation.IsValid());
  ScAddr const relationLast = m_ctx->HelperFindBySystemIdtf("rrel_last");
  EXPECT_TRUE(relationLast.IsValid());

  ScAddr const message = utils::IteratorUtils::getAnyByOutRelation(&*m_ctx, dialog, relation);
  EXPECT_TRUE(message.IsValid());

  ScAddr const messageCopy = utils::IteratorUtils::getAnyByOutRelation(&*m_ctx, dialog, relationLast);
  EXPECT_TRUE(messageCopy.IsValid());

  EXPECT_TRUE(message == messageCopy);
}

TEST_F(ScMemoryTest, wrapInOrientedSetBySequenceRelation)
{
  std::string const data =
      "concept_message"
      "  -> first_message;"
      "  -> second_message; "
      "  -> third_message;; ";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddrVector suggestions;
  ScAddr const firstMessage = m_ctx->HelperFindBySystemIdtf("first_message");
  EXPECT_TRUE(firstMessage.IsValid());
  ScAddr const secondMessage = m_ctx->HelperFindBySystemIdtf("second_message");
  EXPECT_TRUE(secondMessage.IsValid());
  ScAddr const thirdMessage = m_ctx->HelperFindBySystemIdtf("third_message");
  EXPECT_TRUE(thirdMessage.IsValid());
  suggestions.insert(suggestions.end(), {firstMessage, secondMessage, thirdMessage});

  scAgentsCommon::CoreKeynodes::InitGlobal();
  ScAddr orientedSet = utils::GenerationUtils::wrapInOrientedSetBySequenceRelation(&*m_ctx, suggestions);

  std::string const FIRST_MESSAGE_EDGE_ALIAS = "_first_message_edge";
  std::string const SECOND_MESSAGE_EDGE_ALIAS = "_second_message_edge";
  std::string const THIRD_MESSAGE_EDGE_ALIAS = "_third_message_edge";

  ScTemplate findTemplate;
  findTemplate.TripleWithRelation(
      orientedSet,
      ScType::EdgeAccessVarPosPerm >> FIRST_MESSAGE_EDGE_ALIAS,
      firstMessage,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_1);
  findTemplate.TripleWithRelation(
      FIRST_MESSAGE_EDGE_ALIAS,
      ScType::EdgeDCommonVar,
      ScType::EdgeAccessVarPosPerm >> SECOND_MESSAGE_EDGE_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::nrel_basic_sequence);
  findTemplate.Triple(orientedSet, SECOND_MESSAGE_EDGE_ALIAS, secondMessage);
  findTemplate.TripleWithRelation(
      SECOND_MESSAGE_EDGE_ALIAS,
      ScType::EdgeDCommonVar,
      ScType::EdgeAccessVarPosPerm >> THIRD_MESSAGE_EDGE_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::nrel_basic_sequence);
  findTemplate.Triple(orientedSet, THIRD_MESSAGE_EDGE_ALIAS, thirdMessage);

  ScTemplateSearchResult searchResult;
  m_ctx->HelperSearchTemplate(findTemplate, searchResult);
  EXPECT_TRUE(searchResult.Size() == 1);
}

TEST_F(ScMemoryTest, formActionNode)
{
  scAgentsCommon::CoreKeynodes::InitGlobal();

  ScAddr actionClass = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddrVector params = {m_ctx->CreateNode(ScType::NodeConst), m_ctx->CreateNode(ScType::NodeConst)};

  ScAddr const & actionNode = utils::AgentUtils::formActionNode(m_ctx.get(), actionClass, params);
  EXPECT_TRUE(actionNode.IsValid());

  std::string const actionAlias = "_action";

  ScTemplate generatedActionTemplate;
  generatedActionTemplate.Triple(actionClass, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> actionAlias);
  generatedActionTemplate.TripleWithRelation(
      actionAlias,
      ScType::EdgeAccessVarPosPerm,
      params[0],
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_1);
  generatedActionTemplate.TripleWithRelation(
      actionAlias,
      ScType::EdgeAccessVarPosPerm,
      params[1],
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_2);
  ScTemplateSearchResult generatedActionSearchResult;
  m_ctx->HelperSearchTemplate(generatedActionTemplate, generatedActionSearchResult);

  EXPECT_TRUE(generatedActionSearchResult.Size() == 1);
}

TEST_F(ScMemoryTest, getActionResultIfExistForGeneratedAction)
{
  scAgentsCommon::CoreKeynodes::InitGlobal();
  SC_AGENT_REGISTER(scUtilsTestAgents::FinishActionTestAgent)

  ScAddr actionClass = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddrVector params = {m_ctx->CreateNode(ScType::NodeConst), m_ctx->CreateNode(ScType::NodeConst)};

  ScAddr const & answerNode = utils::AgentUtils::getActionResultIfExists(m_ctx.get(), actionClass, params);
  EXPECT_TRUE(answerNode.IsValid());

  std::string const actionAlias = "_action";

  ScTemplate generatedActionTemplate;
  generatedActionTemplate.Triple(actionClass, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> actionAlias);
  generatedActionTemplate.Triple(
      scAgentsCommon::CoreKeynodes::question_finished, ScType::EdgeAccessVarPosPerm, actionAlias);
  generatedActionTemplate.TripleWithRelation(
      actionAlias,
      ScType::EdgeDCommonVar,
      answerNode,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::nrel_answer);
  ScTemplateSearchResult generatedActionSearchResult;
  m_ctx->HelperSearchTemplate(generatedActionTemplate, generatedActionSearchResult);

  EXPECT_TRUE(generatedActionSearchResult.Size() == 1);

  SC_AGENT_UNREGISTER(scUtilsTestAgents::FinishActionTestAgent)
}

TEST_F(ScMemoryTest, getActionResultIfExistForExistingAction)
{
  scAgentsCommon::CoreKeynodes::InitGlobal();
  SC_AGENT_REGISTER(scUtilsTestAgents::FinishActionTestAgent)

  ScAddr actionNode = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const & answerNode = utils::AgentUtils::getActionResultIfExists(m_ctx.get(), actionNode);
  EXPECT_TRUE(answerNode.IsValid());

  ScTemplate generatedActionTemplate;
  generatedActionTemplate.TripleWithRelation(
      actionNode,
      ScType::EdgeDCommonVar,
      answerNode,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::nrel_answer);
  generatedActionTemplate.Triple(
      scAgentsCommon::CoreKeynodes::question_finished, ScType::EdgeAccessVarPosPerm, actionNode);
  ScTemplateSearchResult generatedActionSearchResult;
  m_ctx->HelperSearchTemplate(generatedActionTemplate, generatedActionSearchResult);

  EXPECT_TRUE(generatedActionSearchResult.Size() == 1);

  SC_AGENT_UNREGISTER(scUtilsTestAgents::FinishActionTestAgent)
}

TEST_F(ScMemoryTest, applyGeneratedAction)
{
  scAgentsCommon::CoreKeynodes::InitGlobal();
  SC_AGENT_REGISTER(scUtilsTestAgents::FinishActionTestAgent)

  ScAddr actionClass = m_ctx->CreateNode(ScType::NodeConstClass);
  ScAddrVector params = {m_ctx->CreateNode(ScType::NodeConst), m_ctx->CreateNode(ScType::NodeConst)};

  EXPECT_TRUE(utils::AgentUtils::applyAction(m_ctx.get(), actionClass, params));

  std::string const actionAlias = "_action";

  ScTemplate generatedActionTemplate;
  generatedActionTemplate.Triple(actionClass, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> actionAlias);
  generatedActionTemplate.Triple(
      scAgentsCommon::CoreKeynodes::question_finished, ScType::EdgeAccessVarPosPerm, actionAlias);
  generatedActionTemplate.TripleWithRelation(
      actionAlias,
      ScType::EdgeAccessVarPosPerm,
      params[0],
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_1);
  generatedActionTemplate.TripleWithRelation(
      actionAlias,
      ScType::EdgeAccessVarPosPerm,
      params[1],
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_2);
  ScTemplateSearchResult generatedActionSearchResult;
  m_ctx->HelperSearchTemplate(generatedActionTemplate, generatedActionSearchResult);

  EXPECT_TRUE(generatedActionSearchResult.Size() == 1);

  SC_AGENT_UNREGISTER(scUtilsTestAgents::FinishActionTestAgent)
}

TEST_F(ScMemoryTest, applyExistingAction)
{
  scAgentsCommon::CoreKeynodes::InitGlobal();
  SC_AGENT_REGISTER(scUtilsTestAgents::FinishActionTestAgent)

  ScAddr actionNode = m_ctx->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(utils::AgentUtils::applyAction(m_ctx.get(), actionNode));

  EXPECT_TRUE(m_ctx->HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished, actionNode, ScType::EdgeAccessConstPosPerm));

  SC_AGENT_UNREGISTER(scUtilsTestAgents::FinishActionTestAgent)
}
