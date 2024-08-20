#include "sc_test.hpp"
#include "dummy_file_interface.hpp"

#include "sc-memory/sc_agent.hpp"

#include "test_sc_specificated_agent.hpp"
#include "test_sc_module.hpp"

using ScAgentBuilderTest = ScMemoryTest;

static std::string const agentInitiationCondition = R"(
  @contour = [*
    action_initiated _-> _action;;
    test_specificated_agent_action _-> _action;;
  *];; 

  @contour => nrel_system_identifier: [test_specificated_agent_initiation_condition];;
)";

static std::string const agentResultCondition = R"(
  @contour = [*
    action_initiated _-> _action;;
    test_specificated_agent_action _-> _action;;
    _action _=> nrel_result:: _result;;
  *];; 

  @contour => nrel_system_identifier: [test_specificated_agent_result_condition];;
)";

static std::string const ATestSpecificatedAgentSpecification = R"(
  test_specificated_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_add_outgoing_arc => action_initiated);
  => nrel_sc_agent_action_class: 
    test_specificated_agent_action;
  => nrel_initiation_condition_and_result: 
    (test_specificated_agent_initiation_condition => test_specificated_agent_result_condition);
  <= nrel_sc_agent_key_sc_elements: 
    {
      action_initiated;
      action;
      test_specificated_agent_action
    };
  => nrel_inclusion: 
    ATestSpecificatedAgent
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  test_specificated_agent_initiation_condition_new
  = [*
    action_initiated _-> _action;;
    test_specificated_agent_action _-> _action;;
  *];;

  test_specificated_agent_result_condition_new
  = [*
    action_initiated _-> _action;;
    test_specificated_agent_action _-> _action;;
    _action _=> nrel_result:: _result;;
  *];;

  test_specificated_agent_action
  <- sc_node_class;
  <= nrel_inclusion: sc_action;;
)";

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentHasFullSpecification)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);
  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specificated_agent_action", ScType::NodeConstClass);

  {
    SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
    EXPECT_TRUE(helper.GenerateBySCsText(agentInitiationCondition));
  }
  ScAddr const & initiationConditionAddr =
      m_ctx->HelperFindBySystemIdtf("test_specificated_agent_initiation_condition");

  {
    SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
    EXPECT_TRUE(helper.GenerateBySCsText(agentResultCondition));
  }
  ScAddr const & resultConditionAddr = m_ctx->HelperFindBySystemIdtf("test_specificated_agent_result_condition");

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_add_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(actionClassAddr)
      ->SetInitiationConditionAndResult({initiationConditionAddr, resultConditionAddr})
      ->FinishBuild();
  module.Register(&*m_ctx);

  m_ctx->CreateAction(actionClassAddr).SetArguments().Initiate();
  EXPECT_TRUE(ATestSpecificatedAgent::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentHasAlreadyFullSpecification)
{
  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const & abstractAgentAddr = m_ctx->HelperFindBySystemIdtf("test_specificated_agent");
  ScAddr const & actionClassAddr = m_ctx->HelperFindBySystemIdtf("test_specificated_agent_action");

  ScAddr const & initiationConditionAddr =
      m_ctx->HelperFindBySystemIdtf("test_specificated_agent_initiation_condition_new");
  ScAddr const & resultConditionAddr = m_ctx->HelperFindBySystemIdtf("test_specificated_agent_result_condition_new");

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_add_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(actionClassAddr)
      ->SetInitiationConditionAndResult({initiationConditionAddr, resultConditionAddr})
      ->FinishBuild();
  module.Register(&*m_ctx);

  m_ctx->CreateAction(actionClassAddr).SetArguments().Initiate();
  EXPECT_TRUE(ATestSpecificatedAgent::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentHasFullSpecificationWithTemplateKeynodes)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_add_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(ATestSpecificatedAgent::test_specificated_agent_action)
      ->SetInitiationConditionAndResult(
          {ATestSpecificatedAgent::test_specificated_agent_initiation_condition,
           ATestSpecificatedAgent::test_specificated_agent_result_condition})
      ->FinishBuild();
  module.Register(&*m_ctx);

  m_ctx->CreateAction(ATestSpecificatedAgent::test_specificated_agent_action).SetArguments().Initiate();
  EXPECT_TRUE(ATestSpecificatedAgent::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentHasNotSpecifiedAbstractAgent)
{
  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentHasInvalidAbstractAgent)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()->SetAbstractAgent(abstractAgentAddr)->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentHasNotSpecifiedPrimaryInitiationCondition)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()->SetAbstractAgent(abstractAgentAddr)->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentHasNotSpecifiedActionClass)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_add_outgoing_arc, ScKeynodes::action_initiated})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentHasNotSpecifiedInitiationConditionAndResult)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);
  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specificated_agent_action", ScType::NodeConstClass);

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_add_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(actionClassAddr)
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentSetInvalidAbstractAgent)
{
  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()->SetAbstractAgent(ScAddr{2421421})->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentSetInvalidEventClass)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScAddr{2421421}, ScKeynodes::action_initiated})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentSetInvalidEventSubscriptionElement)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_add_outgoing_arc, ScAddr{2421421}})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentSetInvalidActionClass)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_add_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(ScAddr{2421421})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentSetInvalidInitiationCondition)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);
  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specificated_agent_action", ScType::NodeConstClass);

  ScAddr const & resultConditionAddr =
      m_ctx->HelperResolveSystemIdtf("test_specificated_agent_result_condition", ScType::NodeConstStruct);

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_add_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(actionClassAddr)
      ->SetInitiationConditionAndResult({ScAddr{2421421}, resultConditionAddr})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecificatedAgentSetInvalidConditionResult)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);
  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specificated_agent_action", ScType::NodeConstClass);

  ScAddr const & initiationConditionAddr =
      m_ctx->HelperResolveSystemIdtf("test_specificated_agent_initiation_condition", ScType::NodeConstStruct);

  TestModule module;
  module.AgentBuilder<ATestSpecificatedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_add_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(actionClassAddr)
      ->SetInitiationConditionAndResult({initiationConditionAddr, ScAddr{2421421}})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}
