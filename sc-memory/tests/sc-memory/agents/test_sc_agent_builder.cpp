#include "sc_test.hpp"
#include "dummy_file_interface.hpp"

#include "sc-memory/sc_agent.hpp"

#include "test_sc_specified_agent.hpp"
#include "test_sc_module.hpp"

using ScAgentBuilderTest = ScMemoryTest;

static std::string const agentInitiationCondition = R"(
  @contour_1 = [*
    action_initiated _-> _action;;
    test_specified_agent_action _-> _action;;
  *];; 

  @contour_1 => nrel_system_identifier: [test_specified_agent_initiation_condition];;
)";

static std::string const agentResultCondition = R"(
  @contour_2 = [*
    concept_set _-> _...;;
  *];; 

  @contour_2 => nrel_system_identifier: [test_specified_agent_result_condition];;
)";

static std::string const ATestSpecifiedAgentSpecification = R"(
  test_specified_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_after_generate_outgoing_arc => action_initiated);
  => nrel_sc_agent_action_class: 
    test_specified_agent_action;
  => nrel_initiation_condition_and_result: 
    (test_specified_agent_initiation_condition_new => test_specified_agent_result_condition_new);
  <= nrel_sc_agent_key_sc_elements: 
    {
      action_initiated;
      action;
      test_specified_agent_action
    };
  => nrel_inclusion: 
    ATestSpecifiedAgent
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  test_specified_agent_initiation_condition_new
  = [*
    action_initiated _-> _action;;
    test_specified_agent_action _-> _action;;
  *];;

  test_specified_agent_result_condition_new
  = [*
    concept_set _-> _...;;
  *];;

  test_specified_agent_action
  <- sc_node_class;
  <= nrel_inclusion: action;;
)";

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentHasFullSpecification)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);
  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specified_agent_action", ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::action, actionClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);

  {
    SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
    EXPECT_TRUE(helper.GenerateBySCsText(agentInitiationCondition));
  }
  ScAddr const & initiationConditionAddr = m_ctx->HelperFindBySystemIdtf("test_specified_agent_initiation_condition");

  {
    SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
    EXPECT_TRUE(helper.GenerateBySCsText(agentResultCondition));
  }
  ScAddr const & resultConditionAddr = m_ctx->HelperFindBySystemIdtf("test_specified_agent_result_condition");

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(actionClassAddr)
      ->SetInitiationConditionAndResult({initiationConditionAddr, resultConditionAddr})
      ->FinishBuild();
  module.Register(&*m_ctx);

  m_ctx->CreateAction(actionClassAddr).SetArguments().Initiate();
  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentHasAlreadyFullSpecification)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecifiedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const & abstractAgentAddr = m_ctx->HelperFindBySystemIdtf("test_specified_agent");
  ScAddr const & actionClassAddr = m_ctx->HelperFindBySystemIdtf("test_specified_agent_action");

  ScAddr const & initiationConditionAddr =
      m_ctx->HelperFindBySystemIdtf("test_specified_agent_initiation_condition_new");
  ScAddr const & resultConditionAddr = m_ctx->HelperFindBySystemIdtf("test_specified_agent_result_condition_new");

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(actionClassAddr)
      ->SetInitiationConditionAndResult({initiationConditionAddr, resultConditionAddr})
      ->FinishBuild();
  module.Register(&*m_ctx);

  m_ctx->CreateAction(actionClassAddr).SetArguments().Initiate();
  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentHasFullSpecificationWithTemplateKeynodes)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specified_agent_action", ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::action, actionClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(ATestSpecifiedAgent::test_specified_agent_action)
      ->SetInitiationConditionAndResult(
          {ATestSpecifiedAgent::test_specified_agent_initiation_condition,
           ATestSpecifiedAgent::test_specified_agent_result_condition})
      ->FinishBuild();
  module.Register(&*m_ctx);

  m_ctx->CreateAction(ATestSpecifiedAgent::test_specified_agent_action).SetArguments().Initiate();
  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentHasFullSpecificationWithTemplateKeynodesInKB)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specified_agent_action", ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::action, actionClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(ATestSpecifiedAgent::test_specified_agent_action)
      ->SetInitiationConditionAndResult(
          {ATestSpecifiedAgent::test_specified_agent_initiation_condition_in_kb,
           ATestSpecifiedAgent::test_specified_agent_result_condition_in_kb})
      ->FinishBuild();
  module.Register(&*m_ctx);

  m_ctx->CreateAction(ATestSpecifiedAgent::test_specified_agent_action).SetArguments().Initiate();
  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentHasNotSpecifiedAbstractAgent)
{
  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentHasInvalidAbstractAgent)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(ATestSpecifiedAgent::test_specified_agent_action)
      ->SetInitiationConditionAndResult(
          {ATestSpecifiedAgent::test_specified_agent_initiation_condition_in_kb,
           ATestSpecifiedAgent::test_specified_agent_result_condition_in_kb})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentHasNotSpecifiedPrimaryInitiationCondition)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specified_agent_action", ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::action, actionClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetActionClass(ATestSpecifiedAgent::test_specified_agent_action)
      ->SetInitiationConditionAndResult(
          {ATestSpecifiedAgent::test_specified_agent_initiation_condition_in_kb,
           ATestSpecifiedAgent::test_specified_agent_result_condition_in_kb})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentHasNotSpecifiedActionClass)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetInitiationConditionAndResult(
          {ATestSpecifiedAgent::test_specified_agent_initiation_condition_in_kb,
           ATestSpecifiedAgent::test_specified_agent_result_condition_in_kb})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentHasNotSpecifiedInitiationConditionAndResult)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);
  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specified_agent_action", ScType::NodeConstClass);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, ScKeynodes::action, actionClassAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_inclusion, arcAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(actionClassAddr)
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidState);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentSetInvalidAbstractAgent)
{
  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(ScAddr{2421421})
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(ATestSpecifiedAgent::test_specified_agent_action)
      ->SetInitiationConditionAndResult(
          {ATestSpecifiedAgent::test_specified_agent_initiation_condition_in_kb,
           ATestSpecifiedAgent::test_specified_agent_result_condition_in_kb})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentSetInvalidEventClass)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScAddr{2421421}, ScKeynodes::action_initiated})
      ->SetActionClass(ATestSpecifiedAgent::test_specified_agent_action)
      ->SetInitiationConditionAndResult(
          {ATestSpecifiedAgent::test_specified_agent_initiation_condition_in_kb,
           ATestSpecifiedAgent::test_specified_agent_result_condition_in_kb})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentSetInvalidEventSubscriptionElement)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScAddr{2421421}})
      ->SetActionClass(ATestSpecifiedAgent::test_specified_agent_action)
      ->SetInitiationConditionAndResult(
          {ATestSpecifiedAgent::test_specified_agent_initiation_condition_in_kb,
           ATestSpecifiedAgent::test_specified_agent_result_condition_in_kb})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentSetInvalidActionClass)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(ScAddr{2421421})

      ->SetInitiationConditionAndResult(
          {ATestSpecifiedAgent::test_specified_agent_initiation_condition_in_kb,
           ATestSpecifiedAgent::test_specified_agent_result_condition_in_kb})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentSetInvalidInitiationCondition)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);
  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specified_agent_action", ScType::NodeConstClass);

  ScAddr const & resultConditionAddr =
      m_ctx->HelperResolveSystemIdtf("test_specified_agent_result_condition", ScType::NodeConstStruct);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(actionClassAddr)
      ->SetInitiationConditionAndResult({ScAddr{2421421}, resultConditionAddr})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentBuilderTest, ProgrammlySpecifiedAgentSetInvalidConditionResult)
{
  ScAddr const & abstractAgentAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::abstract_sc_agent, abstractAgentAddr);
  ScAddr const & actionClassAddr =
      m_ctx->HelperResolveSystemIdtf("test_specified_agent_action", ScType::NodeConstClass);

  ScAddr const & initiationConditionAddr =
      m_ctx->HelperResolveSystemIdtf("test_specified_agent_initiation_condition", ScType::NodeConstStruct);

  TestModule module;
  module.AgentBuilder<ATestSpecifiedAgent>()
      ->SetAbstractAgent(abstractAgentAddr)
      ->SetPrimaryInitiationCondition({ScKeynodes::sc_event_after_generate_outgoing_arc, ScKeynodes::action_initiated})
      ->SetActionClass(actionClassAddr)
      ->SetInitiationConditionAndResult({initiationConditionAddr, ScAddr{2421421}})
      ->FinishBuild();
  EXPECT_THROW(module.Register(&*m_ctx), utils::ExceptionInvalidParams);
}
