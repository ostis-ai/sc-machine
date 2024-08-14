#include "agents_test_utils.hpp"

#include "sc-memory/sc_agent.hpp"

#include "test_sc_agent.hpp"
#include "test_sc_module.hpp"

TEST_F(ScAgentTest, InvalidSubscription)
{
  ScAddr const & subscriptionElementAddr{1233241};
  EXPECT_THROW(m_ctx->SubscribeAgent<ATestAddInputArc>(subscriptionElementAddr), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->UnsubscribeAgent<ATestAddInputArc>(subscriptionElementAddr), utils::ExceptionInvalidParams);
}

TEST_F(ScAgentTest, SubscribeAgentTwice)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(m_ctx->SubscribeAgent<ATestAddInputArc>(subscriptionElementAddr));
  EXPECT_THROW(m_ctx->SubscribeAgent<ATestAddInputArc>(subscriptionElementAddr), utils::ExceptionInvalidState);
  EXPECT_NO_THROW(m_ctx->UnsubscribeAgent<ATestAddInputArc>(subscriptionElementAddr));
}

TEST_F(ScAgentTest, UnsubscribeAgentTwice)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_NO_THROW(m_ctx->SubscribeAgent<ATestAddInputArc>(subscriptionElementAddr));
  EXPECT_NO_THROW(m_ctx->UnsubscribeAgent<ATestAddInputArc>(subscriptionElementAddr));
  EXPECT_THROW(m_ctx->UnsubscribeAgent<ATestAddInputArc>(subscriptionElementAddr), utils::ExceptionInvalidState);
}

TEST_F(ScAgentTest, UnsubscribeNotSubscribedAgent)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->UnsubscribeAgent<ATestAddInputArc>(subscriptionElementAddr), utils::ExceptionInvalidState);
}

TEST_F(ScAgentTest, ATestAddInputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->SubscribeAgent<ATestAddInputArc>(subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, subscriptionElementAddr);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddInputArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestAddInputArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestAddOutputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->SubscribeAgent<ATestAddOutputArc>(subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestAddOutputArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestAddEdge)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);
  m_ctx->SubscribeAgent<ATestAddEdge>(subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeUCommonConst, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddEdge::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestAddEdge>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestRemoveInputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, node, subscriptionElementAddr);
  EXPECT_TRUE(e.IsValid());

  m_ctx->SubscribeAgent<ATestRemoveInputArc>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveInputArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestRemoveInputArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestRemoveOutputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());

  m_ctx->SubscribeAgent<ATestRemoveOutputArc>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveOutputArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestRemoveOutputArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestRemoveEdge)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeUCommonConst, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());

  m_ctx->SubscribeAgent<ATestRemoveEdge>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(e));
  EXPECT_TRUE(ATestRemoveEdge::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestRemoveEdge>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestRemoveElement)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  m_ctx->SubscribeAgent<ATestRemoveElement>(subscriptionElementAddr);

  EXPECT_TRUE(m_ctx->EraseElement(subscriptionElementAddr));
  EXPECT_TRUE(ATestRemoveElement::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestRemoveElement>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestChangeLinkContent)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateLink(ScType::LinkConst);

  m_ctx->SubscribeAgent<ATestChangeLinkContent>(subscriptionElementAddr);

  uint32_t const value = 100;
  ScStreamPtr const stream = ScStreamMakeRead(value);
  EXPECT_TRUE(m_ctx->SetLinkContent(subscriptionElementAddr, stream));
  EXPECT_TRUE(ATestChangeLinkContent::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestChangeLinkContent>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestAddMultipleOutputArc)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  m_ctx->SubscribeAgent<ATestAddMultipleOutputArc>(subscriptionElementAddr);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());

  ScAddr const e1 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e1.IsValid());
  EXPECT_TRUE(ATestAddMultipleOutputArc::msWaiter.Wait());

  ScAddr const e2 = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e2.IsValid());
  EXPECT_TRUE(ATestAddMultipleOutputArc::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestAddMultipleOutputArc>(subscriptionElementAddr);
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgumentV1)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArgument(1, ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlyFirstArgumentV2)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  EXPECT_TRUE(m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action)
                  .SetArgument(1, ATestAddOutputArc::add_output_arc_action)
                  .InitiateAndWait(2000));

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgumentV1)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArgument(2, ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultOnlySecondArgumentV2)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  EXPECT_TRUE(m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action)
                  .SetArgument(2, ATestAddOutputArc::add_output_arc_action)
                  .InitiateAndWait(2000));

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultTwoArgumentsV1)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArguments(ATestAddOutputArc::add_output_arc_action, ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResult::msWaiter.Wait());

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestCheckResultTwoArgumentsV2)
{
  m_ctx->SubscribeAgent<ATestCheckResult>();

  EXPECT_TRUE(m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action)
                  .SetArguments(ATestAddOutputArc::add_output_arc_action, ATestAddOutputArc::add_output_arc_action)
                  .InitiateAndWait(2000));

  m_ctx->UnsubscribeAgent<ATestCheckResult>();
}

TEST_F(ScAgentTest, ATestGetInitiationConditionTemplate)
{
  m_ctx->SubscribeAgent<ATestGetInitiationConditionTemplate>();

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestGetInitiationConditionTemplate::msWaiter.Wait(0.2));

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArguments(ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestGetInitiationConditionTemplate::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestGetInitiationConditionTemplate>();
}

TEST_F(ScAgentTest, ATestCheckInitiationCondition)
{
  m_ctx->SubscribeAgent<ATestCheckInitiationCondition>();

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestCheckInitiationCondition::msWaiter.Wait(0.2));

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArguments(ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckInitiationCondition::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestCheckInitiationCondition>();
}

TEST_F(ScAgentTest, ATestGetResultConditionTemplate)
{
  m_ctx->SubscribeAgent<ATestGetResultConditionTemplate>();

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestGetResultConditionTemplate::msWaiter.Wait(1));

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArguments(ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestGetResultConditionTemplate::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestGetResultConditionTemplate>();
}

TEST_F(ScAgentTest, ATestCheckResultCondition)
{
  m_ctx->SubscribeAgent<ATestCheckResultCondition>();

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestCheckResultCondition::msWaiter.Wait(1));

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action)
      .SetArguments(ATestAddOutputArc::add_output_arc_action)
      .Initiate();

  EXPECT_TRUE(ATestCheckResultCondition::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestCheckResultCondition>();
}

TEST_F(ScAgentTest, ActionDeactivated)
{
  m_ctx->SubscribeAgent<ATestActionDeactivated>();

  ScAddr const & arcAddr = m_ctx->CreateEdge(
      ScType::EdgeAccessConstPosPerm, ScKeynodes::action_deactivated, ATestAddOutputArc::add_output_arc_action);

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_FALSE(ATestActionDeactivated::msWaiter.Wait(0.2));

  m_ctx->EraseElement(arcAddr);

  m_ctx->CreateAction(ATestAddOutputArc::add_output_arc_action).SetArguments().Initiate();

  EXPECT_TRUE(ATestActionDeactivated::msWaiter.Wait(1));

  m_ctx->UnsubscribeAgent<ATestActionDeactivated>();
}

TEST_F(ScAgentTest, RegisterAgentWithinModule)
{
  ScAddr const & subscriptionElementAddr = m_ctx->CreateNode(ScType::NodeConst);

  TestModule module;
  module.Agent<ATestAddOutputArc>(subscriptionElementAddr);
  module.Register(&*m_ctx);

  ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_TRUE(node.IsValid());
  ScAddr const e = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, subscriptionElementAddr, node);
  EXPECT_TRUE(e.IsValid());
  EXPECT_TRUE(ATestAddOutputArc::msWaiter.Wait());

  module.Unregister(&*m_ctx);
}

TEST_F(ScAgentTest, AgentHasNoSpecificationInKb)
{
  ATestCheckResult agent;
  EXPECT_THROW(agent.GetAbstractAgent(), utils::ExceptionInvalidState);
  EXPECT_NO_THROW(agent.GetActionClass());
  EXPECT_EQ(agent.GetEventClass(), ScKeynodes::sc_event_add_output_arc);
  EXPECT_THROW(agent.GetEventSubscriptionElement(), utils::ExceptionInvalidState);
  EXPECT_THROW(agent.GetInitiationCondition(), utils::ExceptionInvalidState);
  EXPECT_THROW(agent.GetResultCondition(), utils::ExceptionInvalidState);
  EXPECT_NO_THROW(agent.GetInitiationConditionTemplate());
  EXPECT_NO_THROW(agent.GetResultConditionTemplate());
  EXPECT_EQ(agent.GetName(), "ATestCheckResult");
}
