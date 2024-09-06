#include <gtest/gtest.h>

#include "sc_test.hpp"
#include "dummy_file_interface.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"

#include "sc-agents-common/utils/GenerationUtils.hpp"

TEST_F(ScMemoryTest, WrapInOrientedSetBySequenceRelation)
{
  std::string const data =
      "concept_message"
      "  -> first_message;"
      "  -> second_message; "
      "  -> third_message;; ";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddrVector suggestions;
  ScAddr const firstMessage = m_ctx->SearchElementBySystemIdentifier("first_message");
  EXPECT_TRUE(firstMessage.IsValid());
  ScAddr const secondMessage = m_ctx->SearchElementBySystemIdentifier("second_message");
  EXPECT_TRUE(secondMessage.IsValid());
  ScAddr const thirdMessage = m_ctx->SearchElementBySystemIdentifier("third_message");
  EXPECT_TRUE(thirdMessage.IsValid());
  suggestions.insert(suggestions.end(), {firstMessage, secondMessage, thirdMessage});

  ScAddr orientedSet = utils::GenerationUtils::wrapInOrientedSetBySequenceRelation(&*m_ctx, suggestions);

  std::string const FIRST_MESSAGE_EDGE_ALIAS = "_first_message_edge";
  std::string const SECOND_MESSAGE_EDGE_ALIAS = "_second_message_edge";
  std::string const THIRD_MESSAGE_EDGE_ALIAS = "_third_message_edge";

  ScTemplate findTemplate;
  findTemplate.Quintuple(
      orientedSet,
      ScType::EdgeAccessVarPosPerm >> FIRST_MESSAGE_EDGE_ALIAS,
      firstMessage,
      ScType::EdgeAccessVarPosPerm,
      ScKeynodes::rrel_1);
  findTemplate.Quintuple(
      FIRST_MESSAGE_EDGE_ALIAS,
      ScType::EdgeDCommonVar,
      ScType::EdgeAccessVarPosPerm >> SECOND_MESSAGE_EDGE_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      ScKeynodes::nrel_basic_sequence);
  findTemplate.Triple(orientedSet, SECOND_MESSAGE_EDGE_ALIAS, secondMessage);
  findTemplate.Quintuple(
      SECOND_MESSAGE_EDGE_ALIAS,
      ScType::EdgeDCommonVar,
      ScType::EdgeAccessVarPosPerm >> THIRD_MESSAGE_EDGE_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      ScKeynodes::nrel_basic_sequence);
  findTemplate.Triple(orientedSet, THIRD_MESSAGE_EDGE_ALIAS, thirdMessage);

  ScTemplateSearchResult searchResult;
  m_ctx->HelperSearchTemplate(findTemplate, searchResult);
  EXPECT_TRUE(searchResult.Size() == 1);
}
