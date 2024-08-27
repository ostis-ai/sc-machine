#include "sc_test.hpp"
#include "dummy_file_interface.hpp"

#include "sc-memory/sc_agent.hpp"

#include "test_sc_specificated_agent.hpp"

using ScSpecificatedAgentTest = ScMemoryTest;

static std::string const ATestSpecificatedAgentSpecification = R"(
  test_specificated_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_after_generate_outgoing_arc => action_initiated);
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
    .._action _=> nrel_result:: .._result;;
  *];;

  test_specificated_agent_action
  <- sc_node_class;
  <= nrel_inclusion: sc_action;;
)";

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasFullSpecification)
{
  ATestSpecificatedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(ATestSpecificatedAgent::test_specificated_agent_implementation);

  ScAddr const & actionClassAddr = m_ctx->HelperFindBySystemIdtf("test_specificated_agent_action");
  ScAddr const & argAddr = m_ctx->CreateNode(ScType::NodeConst);

  m_ctx->CreateAction(actionClassAddr).SetArguments(argAddr).Initiate();
  EXPECT_TRUE(ATestSpecificatedAgent::msWaiter.Wait());

  m_ctx->DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      ATestSpecificatedAgent::test_specificated_agent_implementation);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasNotSpecification)
{
  EXPECT_THROW(
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedPrimaryInitiationConditionTwice)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithNotSpecifiedActionClass)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithInvalidActionClass)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedActionClassTwice)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithNotSpecifiedInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithInvalidInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
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
      (sc_event_after_generate_outgoing_arc => action_initiated);
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithInvalidResultInInitiationConditionAndResult)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
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
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasSpecificationWithSpecifiedInitiationConditionAndResultTwice)
{
  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
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
      .._action _=> nrel_result:: .._result;;
    *];;

    test_specificated_agent_action
    <- sc_node_class;
    <= nrel_inclusion: sc_action;;
  )";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentHasFullSpecificationButConditionsDontContainEventTriple)
{
  ATestSpecificatedAgent::msWaiter.Reset();

  std::string const & data = R"(
    test_specificated_agent
    <- abstract_sc_agent;
    => nrel_primary_initiation_condition: 
      (sc_event_after_generate_outgoing_arc => action_initiated);
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
      .._action _-> rrel_1:: .._parameter;;
    *];;

    ..test_specificated_agent_result
    = [*
      test_specificated_agent_action _-> .._action;;
      action_finished _-> .._action;;
      .._action _-> rrel_1:: .._parameter;;
      .._action _=> nrel_result:: .._result;;
    *];;

    test_specificated_agent_action
    <- sc_node_class;
    <= nrel_inclusion: sc_action;;
  )";
  ;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(ATestSpecificatedAgent::test_specificated_agent_implementation);

  ScAddr const & actionClassAddr = m_ctx->HelperFindBySystemIdtf("test_specificated_agent_action");
  ScAddr const & argAddr = m_ctx->CreateNode(ScType::NodeConst);

  m_ctx->CreateAction(actionClassAddr).SetArguments(argAddr).Initiate();
  EXPECT_TRUE(ATestSpecificatedAgent::msWaiter.Wait());

  m_ctx->DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      ATestSpecificatedAgent::test_specificated_agent_implementation);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentSubscribeTwice)
{
  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(ATestSpecificatedAgent::test_specificated_agent_implementation);
  EXPECT_THROW(
      m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);

  m_ctx->DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      ATestSpecificatedAgent::test_specificated_agent_implementation);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentUnsubscribeTwice)
{
  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(ATestSpecificatedAgent::test_specificated_agent_implementation);

  m_ctx->DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      ATestSpecificatedAgent::test_specificated_agent_implementation);
  EXPECT_THROW(
      m_ctx->DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentUnsubscribeNotSubscribed)
{
  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  EXPECT_THROW(
      m_ctx->DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
          ATestSpecificatedAgent::test_specificated_agent_implementation),
      utils::ExceptionInvalidState);
}

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentUnsuccessfullInitiationConditionPass)
{
  ATestSpecificatedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecificatedAgentSpecification;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(ATestSpecificatedAgent::test_specificated_agent_implementation);

  ScAddr const & actionClassAddr = m_ctx->HelperFindBySystemIdtf("test_specificated_agent_action");
  ScAddr const & argAddr = m_ctx->CreateNode(ScType::NodeConst);

  m_ctx->CreateAction(actionClassAddr).Initiate();
  EXPECT_FALSE(ATestSpecificatedAgent::msWaiter.Wait(0.1));

  m_ctx->DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      ATestSpecificatedAgent::test_specificated_agent_implementation);
}

static std::string const ATestSpecificatedAgentSpecificationErasingEdge = R"(
  test_specificated_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_before_erase_edge => test_set);
  => nrel_sc_agent_action_class: 
    test_specificated_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specificated_agent_condition => ..test_specificated_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      test_set;
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
    test_set _<=> test_relation:: test_other_set;;
  *];;

  ..test_specificated_agent_result
  = [*
    test_other_set _<=> test_relation:: test_set;;
  *];;

  test_specificated_agent_action
  <- sc_node_class;
  <= nrel_inclusion: sc_action;;
)";

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentErasingEdgeHasFullSpecification)
{
  ATestSpecificatedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecificatedAgentSpecificationErasingEdge;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(ATestSpecificatedAgent::test_specificated_agent_implementation);

  ScAddr const & testSetAddr = m_ctx->HelperFindBySystemIdtf("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->HelperFindBySystemIdtf("test_other_set");
  ScAddr const & testRelation = m_ctx->HelperFindBySystemIdtf("test_relation");
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeUCommonConst, testSetAddr, testOtherSetAddr);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

  m_ctx->EraseElement(edgeAddr);

  EXPECT_TRUE(ATestSpecificatedAgent::msWaiter.Wait());

  EXPECT_FALSE(m_ctx->IsElement(edgeAddr));
  EXPECT_FALSE(m_ctx->IsElement(arcAddr));

  m_ctx->DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      ATestSpecificatedAgent::test_specificated_agent_implementation);
}

static std::string const ATestSpecificatedAgentSpecificationErasingConnector = R"(
  test_specificated_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_before_erase_connector => test_set);
  => nrel_sc_agent_action_class: 
    test_specificated_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specificated_agent_condition => ..test_specificated_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      test_set;
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
    test_set _<=> test_relation:: test_other_set;;
  *];;

  ..test_specificated_agent_result
  = [*
    test_other_set _<=> test_relation:: test_set;;
  *];;

  test_specificated_agent_action
  <- sc_node_class;
  <= nrel_inclusion: sc_action;;
)";

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentErasingConnectorHasFullSpecification)
{
  ATestSpecificatedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecificatedAgentSpecificationErasingConnector;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(ATestSpecificatedAgent::test_specificated_agent_implementation);

  ScAddr const & testSetAddr = m_ctx->HelperFindBySystemIdtf("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->HelperFindBySystemIdtf("test_other_set");
  ScAddr const & testRelation = m_ctx->HelperFindBySystemIdtf("test_relation");
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeUCommonConst, testOtherSetAddr, testSetAddr);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

  m_ctx->EraseElement(edgeAddr);

  EXPECT_TRUE(ATestSpecificatedAgent::msWaiter.Wait());

  EXPECT_FALSE(m_ctx->IsElement(edgeAddr));
  EXPECT_FALSE(m_ctx->IsElement(arcAddr));

  m_ctx->DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      ATestSpecificatedAgent::test_specificated_agent_implementation);
}

static std::string const ATestSpecificatedAgentSpecificationGeneratingIncomingArc = R"(
  test_specificated_agent
  <- abstract_sc_agent;
  => nrel_primary_initiation_condition: 
    (sc_event_after_generate_incoming_arc => test_set);
  => nrel_sc_agent_action_class: 
    test_specificated_agent_action;
  => nrel_initiation_condition_and_result: 
    (..test_specificated_agent_condition => ..test_specificated_agent_result);
  <= nrel_sc_agent_key_sc_elements: 
    {
      test_set;
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
    test_other_set _-> test_relation:: test_set;;
  *];;

  ..test_specificated_agent_result
  = [*
    test_other_set _-> test_relation:: test_set;;
  *];;

  test_specificated_agent_action
  <- sc_node_class;
  <= nrel_inclusion: sc_action;;
)";

TEST_F(ScSpecificatedAgentTest, ATestSpecificatedAgentGeneratingIncomingArcHasFullSpecification)
{
  ATestSpecificatedAgent::msWaiter.Reset();

  std::string const & data = ATestSpecificatedAgentSpecificationGeneratingIncomingArc;

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  m_ctx->LoadAndSubscribeAgent<ATestSpecificatedAgent>(ATestSpecificatedAgent::test_specificated_agent_implementation);

  ScAddr const & testSetAddr = m_ctx->HelperFindBySystemIdtf("test_set");
  ScAddr const & testOtherSetAddr = m_ctx->HelperFindBySystemIdtf("test_other_set");
  ScAddr const & testRelation = m_ctx->HelperFindBySystemIdtf("test_relation");
  ScAddr const & edgeAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testOtherSetAddr, testSetAddr);
  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, testRelation, edgeAddr);

  EXPECT_TRUE(ATestSpecificatedAgent::msWaiter.Wait());

  m_ctx->DestroyAndUnsubscribeAgent<ATestSpecificatedAgent>(
      ATestSpecificatedAgent::test_specificated_agent_implementation);
}
