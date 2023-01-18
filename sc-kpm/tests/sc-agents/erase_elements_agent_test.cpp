/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
#include "sc-agents-common/agents/EraseElementsAgent.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"

#include "sc-memory/kpm/sc_agent.hpp"

#include "sc_test.hpp"

using namespace scAgentsCommon;

namespace eraseElementsTest
{
const int WAIT_TIME = 1000;

using EraseElementsTest = ScMemoryTest;

void initialize()
{
  CoreKeynodes::InitGlobal();
  EraseElementsKeynodes::InitGlobal();

  SC_AGENT_REGISTER(EraseElementsAgent)
}

void shutdown()
{
  SC_AGENT_UNREGISTER(EraseElementsAgent)
}

void successfulEraseElementsKB(ScMemoryContext & context)
{
  ScAddr const & testActionNode = context.CreateNode(ScType::NodeConst);
  context.HelperSetSystemIdtf("test_action_node", testActionNode);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::question, testActionNode);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, EraseElementsKeynodes::action_erase_elements, testActionNode);
  ScAddr const & erasableElementsSet = context.CreateNode(ScType::NodeConst);
  ScAddr const & edge = context.CreateEdge(ScType::EdgeAccessConstPosPerm, testActionNode, erasableElementsSet);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::rrel_1, edge);

  ScAddr const & conceptExample1 = context.CreateNode(ScType::NodeConstClass);
  context.HelperSetSystemIdtf("conceptExample1", conceptExample1);
  ScAddr const & conceptExample2 = context.CreateNode(ScType::NodeConstClass);
  ScAddr const & conceptExample3 = context.CreateNode(ScType::NodeConstClass);
  ScAddr const & elementExample1 = context.CreateNode(ScType::NodeConst);
  ScAddr const & elementExample2 = context.CreateNode(ScType::NodeConst);
  ScAddr const & rrelExample1 = context.CreateNode(ScType::NodeConstRole);
  context.HelperSetSystemIdtf("rrelExample1", rrelExample1);
  
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, erasableElementsSet, conceptExample1);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, erasableElementsSet, conceptExample2);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, erasableElementsSet, conceptExample3);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, erasableElementsSet, elementExample1);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, erasableElementsSet, elementExample2);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, erasableElementsSet, rrelExample1);

  context.CreateEdge(ScType::EdgeAccessConstPosPerm, EraseElementsKeynodes::unerasable_elements, conceptExample1);
  context.CreateEdge(ScType::EdgeAccessConstPosPerm, EraseElementsKeynodes::unerasable_elements, rrelExample1);
}

TEST_F(EraseElementsTest, successful_erase_elements)
{
  ScMemoryContext & context = *m_ctx;
  initialize();
  successfulEraseElementsKB(context);
  ScAddr const & testActionNode = context.HelperFindBySystemIdtf("test_action_node");
  const std::string setAlias = "_set";
  const std::string elementAlias = "_element";

  context.CreateEdge(
      ScType::EdgeAccessConstPosPerm,
      CoreKeynodes::question_initiated,
      testActionNode);
  utils::AgentUtils::waitAgentResult(&context, testActionNode, WAIT_TIME);
  ScIterator5Ptr it5 = context.Iterator5(
      testActionNode,
      ScType::EdgeDCommon,
      ScType::Unknown,
      ScType::EdgeAccessConstPosPerm,
      CoreKeynodes::nrel_answer
  );
  if (it5->Next())
  {
    ScTemplate scTemplate;
    scTemplate.Triple(
        it5->Get(2),
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVar >> setAlias
    );
    scTemplate.Triple(
      setAlias,
      ScType::EdgeAccessVarPosPerm,
      ScType::Var >> elementAlias
    );
    ScTemplateSearchResult searchResult;
    context.HelperSearchTemplate(scTemplate, searchResult);
    size_t const searchResultSize = searchResult.Size();
    for(size_t i = 0; i < searchResultSize; ++i)
      EXPECT_TRUE(context.HelperCheckArc(EraseElementsKeynodes::unerasable_elements, searchResult[i][elementAlias], ScType::EdgeAccessConstPosPerm));
  }

  shutdown();
}

}
