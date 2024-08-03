#include "sc_test.hpp"
#include "dummy_file_interface.hpp"

#include "sc-memory/sc_agent.hpp"

#include "test_sc_specificated_agent.hpp"

using ScSpecificatedAgentTest = ScMemoryTest;

static std::string const ATestSpecificatedAgentSpecification = R"(
  test_specificated_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_add_output_arc => action_initiated);
  => nrel_sc_agent_action_class: 
    test_specificated_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specificated_agent_condition => ..test_specificated_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      action_initiated;
      action;
      test_specificated_agent_action
    };
  => nrel_inclusion: 
    test_specificated_agent_implementation
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  ..test_specificated_agent_condition
  = [*
    test_specificated_agent_action _-> .._action;;
    action_initiated _-> .._action;;
    .._action _-> rrel_1:: .._parameter;;
  *];;

  ..test_specificated_agent_result
  = [*
    test_specificated_agent_action _-> .._action;;
    action_initiated _-> .._action;;
    action_finished _-> .._action;;
    .._action _-> rrel_1:: .._parameter;;
    .._action _=> nrel_answer:: .._answer;;
  *];;

  test_specificated_agent_action
  <- sc_node_class;
  <= nrel_inclusion: sc_action;;
)";

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasFullSpecification)
{
  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  BuildAndSubscribeAgent<ATestSpecificatedAgent>(
      &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation);

  ScAddr const & actionClassAddr = m_ctx->HelperFindBySystemIdtf("test_specificated_agent_action");
  ScAddr const & argAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAgentContext context;
  context.CreateAction(actionClassAddr).SetArguments(argAddr).Initiate();
  EXPECT_TRUE(ATestSpecificatedAgent::msWaiter.Wait());

  DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasNotSpecification)
{
  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithNotSpecifiedImplementationClass)
{
  std::string const & data = R"(
    test_specificated_agent
    => nrel_inclusion:
      test_specificated_agent_implementation;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedImplementationClassTwice)
{
  std::string const & data = R"(
    test_specificated_agent
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <- platform_dependent_abstract_sc_agent;;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithNotSpecifiedProgram)
{
  std::string const & data = R"(
    test_specificated_agent
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedProgramTwice)
{
  std::string const & data = R"(
    test_specificated_agent
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWitNotSpecifiedAbstractAgent)
{
  std::string const & data = R"(
    test_specificated_agent_implementation
    <- platform_dependent_abstract_sc_agent;
    <= nrel_sc_agent_program: 
    {
      [] (* => nrel_format: format_github_source_link;; *);
      [] (* => nrel_format: format_github_source_link;; *)
    };;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedAbstractAgentTwice)
{
  std::string const & data = R"(
    test_specificated_agent
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
    test_specificated_agent
    => nrel_inclusion:
      test_specificated_agent_implementation;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithInvalidAbstractAgent)
{
  std::string const & data = R"(
    (test_specificated_agent => test_specificated_agent)
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedAbstractAgentNotHavingClass)
{
  std::string const & data = R"(
    test_specificated_agent
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));
  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedAbstractAgentHavingClassTwice)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    <- abstract_sc_agent;
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithNotSpecifiedPrimaryInitiationCondition)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithInvalidPrimaryInitiationCondition)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: ...;
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithInvalidEventClassInPrimaryInitiationCondition)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition:
      (sc_event => action_initiated);
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedPrimaryInitiationConditionTwice)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_add_output_arc => action_initiated);
    => nrel_primary_initiation_condition: 
      (sc_event_add_output_arc => action_initiated);
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));
  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithNotSpecifiedActionClass)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_add_output_arc => action_initiated);
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithInvalidActionClass)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_add_output_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specificated_agent_action;
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedActionClassTwice)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_add_output_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specificated_agent_action;
    => nrel_sc_agent_action_class: 
      test_specificated_agent_action;
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specificated_agent_action
    <- sc_node_class;
    <= nrel_inclusion: sc_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithNotSpecifiedInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_add_output_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specificated_agent_action;
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specificated_agent_action
    <- sc_node_class;
    <= nrel_inclusion: sc_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithInvalidInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_add_output_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specificated_agent_action;
    => nrel_initiation_condition_and_result: ...;
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specificated_agent_action
    <- sc_node_class;
    <= nrel_inclusion: sc_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(
    ScSpecificatedAgentTest,
    ATestSpecificatedAgentHasSpecificationWithInvalidConditionInInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_add_output_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specificated_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specificated_agent_condition => ..test_specificated_agent_result);
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specificated_agent_action
    <- sc_node_class;
    <= nrel_inclusion: sc_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithInvalidResultInInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_add_output_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specificated_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specificated_agent_condition => ..test_specificated_agent_result);
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    ..test_specificated_agent_condition
    = [*
      test_specificated_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    test_specificated_agent_action
    <- sc_node_class;
    <= nrel_inclusion: sc_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedInitiationConditionAndResultTwice)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_add_output_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specificated_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specificated_agent_condition => ..test_specificated_agent_result);
    => nrel_initiation_condition_and_result: 
      (..test_specificated_agent_condition => ..test_specificated_agent_result);
    => nrel_inclusion:
      test_specificated_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    ..test_specificated_agent_condition
    = [*
      test_specificated_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specificated_agent_result
    = [*
      test_specificated_agent_action _-> .._action;;
      action_finished _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
      .._action _=> nrel_answer:: .._answer;;
    *];;

    test_specificated_agent_action
    <- sc_node_class;
    <= nrel_inclusion: sc_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentSubscribeTwice)
{
  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  BuildAndSubscribeAgent<ATestSpecificatedAgent>(
      &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation);
  EXPECT_THROW(
      BuildAndSubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);

  DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentUnsubscribeTwice)
{
  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  BuildAndSubscribeAgent<ATestSpecificatedAgent>(
      &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation);

  DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation);
  EXPECT_THROW(
      DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentUnsubscribeNotSubscribed)
{
  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
          &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentUnsuccessfullInitiationConditionPass)
{
  ATestSpecificatedAgent::msWaiter = TestWaiter();

  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  BuildAndSubscribeAgent<ATestSpecificatedAgent>(
      &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation);

  ScAddr const & actionClassAddr = m_ctx->HelperFindBySystemIdtf("test_specificated_agent_action");
  ScAddr const & argAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScAgentContext context;
  context.CreateAction(actionClassAddr).Initiate();
  EXPECT_FALSE(ATestSpecificatedAgent::msWaiter.Wait(0.1));

  DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      &*m_ctx, ATestSpecificatedAgent::test_specificated_agent_implementation);
}
