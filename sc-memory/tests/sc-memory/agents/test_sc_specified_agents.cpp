#include "sc_test.hpp"
#include "dummy_file_interface.hpp"

#include "sc-memory/sc_agent.hpp"

#include "test_sc_specified_agent.hpp"

using ScSpecifiedAgentTest = ScMemoryTest;

static std::string const ATestSpecifiedAgentSpecification = R"(
  test_specified_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_after_generate_outgoing_arc => action_initiated);
  => nrel_sc_agent_action_class: 
    test_specified_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specified_agent_condition => ..test_specified_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      action_initiated;
      action;
      test_specified_agent_action
    };
  => nrel_inclusion: 
    test_specified_agent_implementation
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  ..test_specified_agent_condition
  = [*
    test_specified_agent_action _-> .._action;;
    action_initiated _-> .._action;;
    .._action _-> rrel_1:: .._parameter;;
  *];;

  ..test_specified_agent_result
  = [*
    concept_set _-> _...;;
  *];;

  test_specified_agent_action
  <- sc_node_class;
  <= nrel_inclusion: information_action;;
)";

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasFullSpecification)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecifiedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & actionClassAddr = m_ctx->SearchElementBySystemIdentifier("test_specified_agent_action");
  ScAddr const & argAddr = m_ctx->GenerateNode(ScType::NodeConst);

  m_ctx->GenerateAction(actionClassAddr).SetArguments(argAddr).Initiate();
  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

TEST_F(ScSpecifiedAgentTest, SubscribeSpecifiedAgentByInvalidAgentImplementation)
{
  EXPECT_THROW(m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ScAddr::Empty), utils::ExceptionInvalidParams);
}

TEST_F(ScSpecifiedAgentTest, UnsubscribeSpecifiedAgentByInvalidAgentImplementation)
{
  EXPECT_THROW(m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ScAddr::Empty), utils::ExceptionInvalidParams);
}

TEST_F(ScSpecifiedAgentTest, SubscribeSpecifiedAgentTwice)
{
  std::string const & data = ATestSpecifiedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);

  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
}

TEST_F(ScSpecifiedAgentTest, UnsubscribeNotSubscribedSpecifiedAgent)
{
  std::string data = ATestSpecifiedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, SubscribeSpecifiedAgentToSeveralImplementations)
{
  std::string data = ATestSpecifiedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  data = R"(
    test_specified_agent
    => nrel_inclusion: 
      other_test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
  )";

  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  ScAddr const & otherTestSpecifiedAgentImplementation =
      m_ctx->SearchElementBySystemIdentifier("other_test_specified_agent_implementation");
  EXPECT_NO_THROW(m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(otherTestSpecifiedAgentImplementation));

  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  EXPECT_NO_THROW(m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(otherTestSpecifiedAgentImplementation));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasNotSpecification)
{
  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithPlatformIndependentImplementation)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };
    => nrel_inclusion: 
      test_specified_agent_implementation
      (*
        <- platform_independent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));

  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithNotSpecifiedImplementationClass)
{
  std::string const & data = R"(
    test_specified_agent
    => nrel_inclusion:
      test_specified_agent_implementation;;

    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedImplementationHavingDependentClassTwice)
{
  std::string const & data = R"(
    test_specified_agent
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <- platform_dependent_abstract_sc_agent;;
      *);;

    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedImplementationHavingIndependentClassTwice)
{
  std::string const & data = R"(
    test_specified_agent
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_independent_abstract_sc_agent;;
        <- platform_independent_abstract_sc_agent;;
      *);;

    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedImplementationHavingTwoDifferentClasses)
{
  std::string const & data = R"(
    test_specified_agent
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <- platform_independent_abstract_sc_agent;;
      *);;

    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWitNotSpecifiedAbstractAgent)
{
  std::string const & data = R"(
    test_specified_agent_implementation
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
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedAbstractAgentTwice)
{
  std::string const & data = R"(
    test_specified_agent
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;

    test_specified_agent
    <- abstract_sc_agent;
    => nrel_inclusion:
      test_specified_agent_implementation;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSeveralSpecifiedAbstractAgents)
{
  std::string const & data = R"(
    test_specified_agent
    => nrel_inclusion: nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;

    test_specified_agent_invalid
    <- abstract_sc_agent;
    => nrel_inclusion:
      test_specified_agent_implementation;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithInvalidAbstractAgent)
{
  std::string const & data = R"(
    (test_specified_agent => test_specified_agent)
    => nrel_inclusion:
      test_specified_agent_implementation
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
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedAbstractAgentNotHavingClass)
{
  std::string const & data = R"(
    test_specified_agent
    => nrel_inclusion:
      test_specified_agent_implementation
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
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedAbstractAgentHavingClassTwice)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    <- abstract_sc_agent;
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithNotSpecifiedPrimaryInitiationCondition)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithInvalidPrimaryInitiationCondition)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: ...;
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithInvalidEventClassInPrimaryInitiationCondition)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition:
      (sc_event => action_initiated);
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedPrimaryInitiationConditionTwice)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: nrel_primary_initiation_condition:
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));
  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedEventClassHavingClassTwice)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: nrel_primary_initiation_condition:
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;

    sc_event_after_generate_outgoing_arc <- sc_event;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));
  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSeveralSpecifiedPrimaryInitiationConditions)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: nrel_primary_initiation_condition:
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));
  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithNotSpecifiedActionClass)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;
    
    test_specified_agent
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithInvalidActionClass)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action_invalid;
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action_invalid
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action_invalid _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSeveralSpecifiedDifferentActionClasses)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: nrel_sc_agent_action_class:
      test_specified_agent_action;
    => nrel_sc_agent_action_class: 
      test_specified_agent_action_invalid;
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedActionClassTwice)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedActionClassNotHavingSuperClass)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithNotSpecifiedInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithInvalidInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: ...;
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithInvalidConditionInInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithInvalidResultInInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedInitiationConditionAndResultTwice)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_NO_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
  EXPECT_NO_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSeveralSpecifiedInitiationConditionAndResults)
{
  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: nrel_initiation_condition_and_result:
      (..test_specified_agent_condition => ..test_specified_agent_result);
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    => nrel_inclusion:
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      action_initiated _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasFullSpecificationButInitiationConditionDoesntContainEventTriple)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = R"(
    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        action_initiated;
        action;
        test_specified_agent_action
      };
    => nrel_inclusion: 
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    ..test_specified_agent_condition
    = [*
      test_specified_agent_action _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specified_agent_result
    = [*
      concept_set _-> _...;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & actionClassAddr = m_ctx->SearchElementBySystemIdentifier("test_specified_agent_action");
  ScAddr const & argAddr = m_ctx->GenerateNode(ScType::NodeConst);

  m_ctx->GenerateAction(actionClassAddr).SetArguments(argAddr).Initiate();
  EXPECT_FALSE(ATestSpecifiedAgent::msWaiter.Wait(0.2));

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentSubscribeTwice)
{
  std::string const & data = ATestSpecifiedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentUnsubscribeTwice)
{
  std::string const & data = ATestSpecifiedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
  EXPECT_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentUnsubscribeNotSubscribed)
{
  std::string const & data = ATestSpecifiedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentUnsuccessfulInitiationConditionPass)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecifiedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & actionClassAddr = m_ctx->SearchElementBySystemIdentifier("test_specified_agent_action");

  m_ctx->GenerateAction(actionClassAddr).Initiate();
  EXPECT_FALSE(ATestSpecifiedAgent::msWaiter.Wait(0.2));

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

static std::string const ATestSpecifiedAgentSpecificationErasingEdge = R"(
  test_specified_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_before_erase_edge => test_set);
  => nrel_sc_agent_action_class: 
    test_specified_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specified_agent_condition => ..test_specified_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      test_set;
      action;
      test_specified_agent_action
    };
  => nrel_inclusion: 
    test_specified_agent_implementation
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  ..test_specified_agent_condition
  = [*
    test_set _<=> test_relation:: test_other_set;;
  *];;

  ..test_specified_agent_result
  = [*
    concept_set _-> _...;;
  *];;

  test_specified_agent_action
  <- sc_node_class;
  <= nrel_inclusion: information_action;;
)";

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentErasingEdgeHasFullSpecification)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecifiedAgentSpecificationErasingEdge;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & testSetAddr = m_ctx->SearchElementBySystemIdentifier("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->SearchElementBySystemIdentifier("test_other_set");
  ScAddr const & testRelation = m_ctx->SearchElementBySystemIdentifier("test_relation");
  ScAddr const & edgeAddr = m_ctx->GenerateConnector(ScType::EdgeUCommonConst, testSetAddr, testOtherSetAddr);
  m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

  m_ctx->EraseElement(edgeAddr);

  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

static std::string const ATestSpecifiedAgentSpecificationErasingConnector = R"(
  test_specified_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_before_erase_connector => test_set);
  => nrel_sc_agent_action_class: 
    test_specified_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specified_agent_condition => ..test_specified_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      test_set;
      action;
      test_specified_agent_action
    };
  => nrel_inclusion: 
    test_specified_agent_implementation
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  ..test_specified_agent_condition
  = [*
    test_set _<=> test_relation:: test_other_set;;
  *];;

  ..test_specified_agent_result
  = [*
    concept_set _-> _...;;
  *];;

  test_specified_agent_action
  <- sc_node_class;
  <= nrel_inclusion: information_action;;
)";

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentErasingConnectorHasFullSpecification)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecifiedAgentSpecificationErasingConnector;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & testSetAddr = m_ctx->SearchElementBySystemIdentifier("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->SearchElementBySystemIdentifier("test_other_set");
  ScAddr const & testRelation = m_ctx->SearchElementBySystemIdentifier("test_relation");
  ScAddr const & edgeAddr = m_ctx->GenerateConnector(ScType::EdgeUCommonConst, testOtherSetAddr, testSetAddr);
  m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

  m_ctx->EraseElement(edgeAddr);

  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

static std::string const ATestSpecifiedAgentSpecificationErasingOutgoingArcAsConnector = R"(
  test_specified_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_before_erase_connector => test_set);
  => nrel_sc_agent_action_class: 
    test_specified_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specified_agent_condition => ..test_specified_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      test_set;
      action;
      test_specified_agent_action
    };
  => nrel_inclusion: 
    test_specified_agent_implementation
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  ..test_specified_agent_condition
  = [*
    test_set _=> test_relation:: test_other_set;;
  *];;

  ..test_specified_agent_result
  = [*
    concept_set _-> _...;;
  *];;

  test_specified_agent_action
  <- sc_node_class;
  <= nrel_inclusion: information_action;;
)";

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentErasingOutgoingArcAsConnectorHasFullSpecification)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecifiedAgentSpecificationErasingOutgoingArcAsConnector;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & testSetAddr = m_ctx->SearchElementBySystemIdentifier("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->SearchElementBySystemIdentifier("test_other_set");
  ScAddr const & testRelation = m_ctx->SearchElementBySystemIdentifier("test_relation");
  ScAddr const & edgeAddr = m_ctx->GenerateConnector(ScType::EdgeDCommonConst, testSetAddr, testOtherSetAddr);
  m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

  m_ctx->EraseElement(edgeAddr);

  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

static std::string const ATestSpecifiedAgentSpecificationErasingIncomingArcAsConnector = R"(
  test_specified_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_before_erase_connector => test_set);
  => nrel_sc_agent_action_class: 
    test_specified_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specified_agent_condition => ..test_specified_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      test_set;
      action;
      test_specified_agent_action
    };
  => nrel_inclusion: 
    test_specified_agent_implementation
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  ..test_specified_agent_condition
  = [*
    test_set _<= test_relation:: test_other_set;;
  *];;

  ..test_specified_agent_result
  = [*
    concept_set _-> _...;;
  *];;

  test_specified_agent_action
  <- sc_node_class;
  <= nrel_inclusion: information_action;;
)";

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentErasingIncomingArcAsConnectorHasFullSpecification)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecifiedAgentSpecificationErasingIncomingArcAsConnector;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & testSetAddr = m_ctx->SearchElementBySystemIdentifier("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->SearchElementBySystemIdentifier("test_other_set");
  ScAddr const & testRelation = m_ctx->SearchElementBySystemIdentifier("test_relation");
  ScAddr const & edgeAddr = m_ctx->GenerateConnector(ScType::EdgeDCommonConst, testOtherSetAddr, testSetAddr);
  m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

  m_ctx->EraseElement(edgeAddr);

  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

static std::string const ATestSpecifiedAgentSpecificationGeneratingIncomingArc = R"(
  test_specified_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_after_generate_incoming_arc => test_set);
  => nrel_sc_agent_action_class: 
    test_specified_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specified_agent_condition => ..test_specified_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      test_set;
      action;
      test_specified_agent_action
    };
  => nrel_inclusion: 
    test_specified_agent_implementation
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  ..test_specified_agent_condition
  = [*
    test_other_set _-> test_relation:: test_set;;
  *];;

  ..test_specified_agent_result
  = [*
    concept_set _-> _...;;
  *];;

  test_specified_agent_action
  <- sc_node_class;
  <= nrel_inclusion: information_action;;
)";

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentGeneratingIncomingArcHasFullSpecification)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecifiedAgentSpecificationGeneratingIncomingArc;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & testSetAddr = m_ctx->SearchElementBySystemIdentifier("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->SearchElementBySystemIdentifier("test_other_set");
  ScAddr const & testRelation = m_ctx->SearchElementBySystemIdentifier("test_relation");
  ScAddr const & edgeAddr = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, testOtherSetAddr, testSetAddr);
  m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

static std::string const ATestSpecifiedAgentSpecificationChangingLinkContent = R"(
  @test_link = [content];;

  @test_link
  => nrel_system_identifier: [test_link];
  <- lang_en;;

  test_specified_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_before_change_link_content => @test_link);
  => nrel_sc_agent_action_class: 
    test_specified_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specified_agent_condition => ..test_specified_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      @test_link;
      action;
      test_specified_agent_action
    };
  => nrel_inclusion: 
    test_specified_agent_implementation
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  ..test_specified_agent_condition
  = [*
    @test_link _<- lang_en;;
  *];;

  ..test_specified_agent_result
  = [*
  *];;

  test_specified_agent_action
  <- sc_node_class;
  <= nrel_inclusion: information_action;;
)";

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentChangingLinkContentHasFullSpecification)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecifiedAgentSpecificationChangingLinkContent;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & testLinkAddr = m_ctx->SearchElementBySystemIdentifier("test_link");
  m_ctx->SetLinkContent(testLinkAddr, "new content");

  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

static std::string const ATestSpecifiedAgentSpecificationErasingElement = R"(
  test_element
  <- my_class;;

  test_specified_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_before_erase_element => test_element);
  => nrel_sc_agent_action_class: 
    test_specified_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specified_agent_condition => ..test_specified_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      test_element;
      action;
      test_specified_agent_action
    };
  => nrel_inclusion: 
    test_specified_agent_implementation
    (*
      <- platform_dependent_abstract_sc_agent;;
      <= nrel_sc_agent_program: 
      {
        [] (* => nrel_format: format_github_source_link;; *);
        [] (* => nrel_format: format_github_source_link;; *)
      };;
    *);;

  ..test_specified_agent_condition
  = [*
    test_element _<- my_class;;
  *];;

  ..test_specified_agent_result
  = [*
  *];;

  test_specified_agent_action
  <- sc_node_class;
  <= nrel_inclusion: information_action;;
)";

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentErasingElementHasFullSpecification)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecifiedAgentSpecificationErasingElement;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & testElementAddr = m_ctx->SearchElementBySystemIdentifier("test_element");
  m_ctx->EraseElement(testElementAddr);

  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());
}

TEST_F(
    ScSpecifiedAgentTest,
    ATestSpecifiedAgentErasingElementHasFullSpecificationWithFullVarInitiationConditionTemplate)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = R"(
    test_element
    <- my_class;;

    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_before_erase_element => test_element);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        test_element;
        action;
        test_specified_agent_action
      };
    => nrel_inclusion: 
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    ..test_specified_agent_condition
    = [*
      .._element _<- .._class;;
    *];;

    ..test_specified_agent_result
    = [*
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & testElementAddr = m_ctx->SearchElementBySystemIdentifier("test_element");
  m_ctx->EraseElement(testElementAddr);

  EXPECT_FALSE(ATestSpecifiedAgent::msWaiter.Wait(0.2));
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentErasingElementHasFullSpecificationWithFullVarResultConditionTemplate)
{
  ATestSpecifiedAgent::msWaiter.Reset();

  std::string const & data = R"(
    test_element
    <- my_class;;

    test_specified_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_before_erase_element => test_element);
    => nrel_sc_agent_action_class: 
      test_specified_agent_action;
    => nrel_initiation_condition_and_result: 
      (..test_specified_agent_condition => ..test_specified_agent_result);
    <= nrel_sc_agent_key_sc_elements: 
      {
        test_element;
        action;
        test_specified_agent_action
      };
    => nrel_inclusion: 
      test_specified_agent_implementation
      (*
        <- platform_dependent_abstract_sc_agent;;
        <= nrel_sc_agent_program: 
        {
          [] (* => nrel_format: format_github_source_link;; *);
          [] (* => nrel_format: format_github_source_link;; *)
        };;
      *);;

    ..test_specified_agent_condition
    = [*
      test_element _<- my_class;;
    *];;

    ..test_specified_agent_result
    = [*
      .._element _<- .._class;;
    *];;

    test_specified_agent_action
    <- sc_node_class;
    <= nrel_inclusion: information_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);

  ScAddr const & testElementAddr = m_ctx->SearchElementBySystemIdentifier("test_element");
  m_ctx->EraseElement(testElementAddr);

  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());
}
