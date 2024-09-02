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

  ScAddr const & actionClassAddr = m_ctx->HelperFindBySystemIdtf("test_specified_agent_action");
  ScAddr const & argAddr = m_ctx->CreateNode(ScType::NodeConst);

  m_ctx->CreateAction(actionClassAddr).SetArguments(argAddr).Initiate();
  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasNotSpecification)
{
  EXPECT_THROW(
      m_ctx->SubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation),
      utils::ExceptionInvalidState);
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

TEST_F(ScSpecifiedAgentTest, ATestSpecifiedAgentHasSpecificationWithSpecifiedImplementationClassTwice)
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

  ScAddr const & actionClassAddr = m_ctx->HelperFindBySystemIdtf("test_specified_agent_action");
  ScAddr const & argAddr = m_ctx->CreateNode(ScType::NodeConst);

  m_ctx->CreateAction(actionClassAddr).SetArguments(argAddr).Initiate();
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

  ScAddr const & actionClassAddr = m_ctx->HelperFindBySystemIdtf("test_specified_agent_action");

  m_ctx->CreateAction(actionClassAddr).Initiate();
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

  ScAddr const & testSetAddr = m_ctx->HelperFindBySystemIdtf("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->HelperFindBySystemIdtf("test_other_set");
  ScAddr const & testRelation = m_ctx->HelperFindBySystemIdtf("test_relation");
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeUCommonConst, testSetAddr, testOtherSetAddr);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

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

  ScAddr const & testSetAddr = m_ctx->HelperFindBySystemIdtf("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->HelperFindBySystemIdtf("test_other_set");
  ScAddr const & testRelation = m_ctx->HelperFindBySystemIdtf("test_relation");
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeUCommonConst, testOtherSetAddr, testSetAddr);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

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

  ScAddr const & testSetAddr = m_ctx->HelperFindBySystemIdtf("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->HelperFindBySystemIdtf("test_other_set");
  ScAddr const & testRelation = m_ctx->HelperFindBySystemIdtf("test_relation");
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testOtherSetAddr, testSetAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

  EXPECT_TRUE(ATestSpecifiedAgent::msWaiter.Wait());

  m_ctx->UnsubscribeSpecifiedAgent<ATestSpecifiedAgent>(ATestSpecifiedAgent::test_specified_agent_implementation);
}
