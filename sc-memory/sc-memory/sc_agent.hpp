/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "sc_agent_builder.hpp"

#include "utils/sc_log.hpp"

//! Log functions to be used in agent class methods.
#define SC_AGENT_LOG_DEBUG(__msg__) SC_LOG_DEBUG(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_INFO(__msg__) SC_LOG_INFO(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_WARNING(__msg__) SC_LOG_WARNING(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_ERROR(__msg__) SC_LOG_ERROR(GetName() << ": " << __msg__)

template <class TScEvent>
class ScElementaryEventSubscription;
class ScAction;
class ScResult;

/*!
 * @class ScAgentBase
 * @brief An abstract base class for agents.
 *
 * This class provides a base implementation for agents, offering methods for initialization, shutdown, and handling
 * events.
 *
 * @tparam TScEvent The type of sc-event this agent handles.
 * @tparam TScContext The type of sc-memory context that used by agent.
 */
template <class TScEvent, class TScContext = ScAgentContext>
class _SC_EXTERN ScAgentBase : public ScObject
{
  static_assert(std::is_base_of<class ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");
  static_assert(
      std::is_base_of<class ScAgentContext, TScContext>::value,
      "TScContext type must be derived from ScAgentContext type.");
  static_assert(std::is_move_constructible<TScContext>::value, "TScContext type must be movable constructible.");

  template <class TScAgent>
  friend class ScAgentBuilder;

public:
  using TEventType = TScEvent;

  _SC_EXTERN ~ScAgentBase() noexcept override;

  /*!
   * @brief Gets abstract agent for agent of this class.
   *
   * This method searches in knowledge base the following construction
   *
   * @code
   *             nrel_inclusion
   *                  |
   *                  |
   *                  |
   *                  \/
   * abstract_agent ======> agent_implementation
   * @endcode
   *
   * and returns `abstract_agent`.
   *
   * @return A sc-address of abstract agent.
   * @throws utils::ExceptionInvalidState if the agent implementation for this agent class is not included in any
   * abstract sc-agent.
   */
  _SC_EXTERN virtual ScAddr GetAbstractAgent() const noexcept(false);

  /*!
   * @brief Gets the sc-event class to which the agent class is subscribed.
   *
   * This method searches in knowledge base the following construction
   *
   * @code
   *   nrel_primary_initiation_condition
   *                  |
   *                  |
   *                  |  event_class
   *                  \/     ||
   * abstract_agent =======> ||
   *                         ||
   *                         \/
   *               event_subscription_element
   * @endcode
   *
   * and returns `event_class`.
   *
   * @return A sc-address of the event class.
   * @throws utils::ExceptionInvalidState if the abstract sc-agent for this agent class does not have a primary
   * initiation condition.
   */
  _SC_EXTERN virtual ScAddr GetEventClass() const noexcept(false);

  /*!
   * @brief Gets sc-event subscription sc-element for which sc-event initiates.
   *
   * This method searches in knowledge base the following construction
   *
   * @code
   *   nrel_primary_initiation_condition
   *                  |
   *                  |
   *                  |  event_class
   *                  \/     ||
   * abstract_agent =======> ||
   *                         ||
   *                         \/
   *               event_subscription_element
   * @endcode
   *
   * and returns `event_subscription_element`.
   *
   * @return A sc-address of sc-event subscription sc-element.
   * @throws utils::ExceptionInvalidState if the abstract sc-agent for this agent class does not have a primary
   * initiation condition.
   */
  _SC_EXTERN virtual ScAddr GetEventSubscriptionElement() const noexcept(false);

  /*!
   * @brief Gets action class that the agent performs.
   *
   * This method searches in knowledge base the following construction
   *
   * @code
   *            nrel_sc_agent_action_class
   *                        |
   *                        |
   *                        |
   *                        \/
   * agent_implementation ======> action_class
   * @endcode
   *
   * and returns `action_class`.
   *
   * @return A sc-address of the action class.
   * @throws utils::ExceptionInvalidState if the abstract sc-agent for this agent class does not have an action class.
   */
  _SC_EXTERN virtual ScAddr GetActionClass() const noexcept(false);

  /*!
   * @brief Checks initiation condition for agent of this class.
   * @param event An initiated sc-event to which the agent reacted.
   * @return true if initiation condition was checked successfully, otherwise true.
   */
  _SC_EXTERN virtual bool CheckInitiationCondition(TScEvent const & event);

  /*!
   * @brief Gets initiation condition for agent of this class.
   *
   * This method searches in knowledge base the following construction
   *
   * @code
   *   nrel_initiation_condition_and_result
   *                  |
   *                  |
   *                  | initiation_condition
   *                  \/       ||
   * abstract_agent =========> ||
   *                           ||
   *                           \/
   *                      result_condition
   * @endcode
   *
   * and returns `initiation_condition`.
   *
   * @return A sc-address of initiation condition.
   * @throws utils::ExceptionInvalidState if the abstract sc-agent for this agent class does not have an initiation
   * condition.
   */
  _SC_EXTERN virtual ScAddr GetInitiationCondition() const noexcept(false);

  /*!
   * @brief Gets initiation condition template represented in program.
   * @return A sc-template of initiation condition.
   */
  _SC_EXTERN virtual ScTemplate GetInitiationConditionTemplate() const;

  /*!
   * @brief Executes the program associated with the agent.
   * @param event A sc-event that triggered the agent.
   * @param action A sc-action to be performed by the agent.
   * @return A result of the program execution.
   */
  _SC_EXTERN virtual ScResult DoProgram(TScEvent const & event, ScAction & action);

  /*!
   * @brief Executes the program associated with the agent.
   * @param action A sc-action to be performed by the agent.
   * @return A result of the program execution.
   */
  _SC_EXTERN virtual ScResult DoProgram(ScAction & action);

  /*!
   * @brief Gets the result of the agent's execution.
   * @param event An initiated sc-event to which the agent reacted.
   * @param action A sc-action that the agent performs.
   * @return true if result condition was checked successfully, otherwise true.
   */
  _SC_EXTERN virtual bool CheckResultCondition(TScEvent const & event, ScAction & action);

  /*!
   * @brief Gets result condition for agent of this class.
   *
   * This method searches in knowledge base the following construction
   *
   * @code
   *   nrel_initiation_condition_and_result
   *                  |
   *                  |
   *                  | initiation_condition
   *                  \/       ||
   * abstract_agent =========> ||
   *                           ||
   *                           \/
   *                      result_condition
   * @endcode
   *
   * and returns `result_condition`.
   *
   * @return A sc-address of result condition.
   * @throws utils::ExceptionInvalidState if the abstract sc-agent for this agent class does not have an result
   * condition.
   */
  _SC_EXTERN virtual ScAddr GetResultCondition() const noexcept(false);

  /*!
   * @brief Gets result condition template represented in program.
   * @return A sc-template of result condition.
   */
  _SC_EXTERN virtual ScTemplate GetResultConditionTemplate() const;

protected:
  mutable TScContext m_memoryCtx;
  ScAddr m_agentImplementationAddr;

  static inline std::unordered_map<std::string, ScAddrToValueUnorderedMap<ScEventSubscription *>> m_events;

  _SC_EXTERN ScAgentBase() noexcept;

  /*!
   * @brief Sets initiator of the agent.
   * @param userAddr A sc-address of user that initiated this agent.
   */
  _SC_EXTERN void SetInitiator(ScAddr const & userAddr) noexcept;

  /*!
   * @brief Sets the implementation of the agent of this class.
   * @param agentImplementationAddr A sc-address of the agent implementation.
   */
  _SC_EXTERN void SetImplementation(ScAddr const & agentImplementationAddr) noexcept;

  /*!
   * @brief Checks if the agent may be specified in knowledge base.
   * @return true if the agent has implementation in knowledge base, otherwise true.
   */
  _SC_EXTERN bool MayBeSpecified() const noexcept;

  static _SC_EXTERN std::function<void(TScEvent const &)> GetCallback(ScAddr const & agentImplementationAddr) noexcept;
};

/*!
 * @class ScAgent
 * @brief A class for agents that can subscribe to any elementary sc-events.
 *
 * This class extends ScAgentBase and provides methods for subscribing and unsubscribing
 * to sc-events.
 *
 * @tparam TScEvent The type of sc-event this agent class handles.
 * @tparam TScContext The type of sc-memory context that used by agent.
 */
template <class TScEvent, class TScContext = ScAgentContext>
class _SC_EXTERN ScAgent : public ScAgentBase<TScEvent, TScContext>
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");
  static_assert(
      std::is_base_of<class ScAgentContext, TScContext>::value,
      "TScContext type must be derived from ScAgentContext type.");
  static_assert(std::is_move_constructible<TScContext>::value, "TScContext type must be movable constructible.");

  template <typename TScAgent>
  struct HasOverride
  {
    struct CheckInitiationCondition
    {
      static constexpr bool value =
          !std::is_same<decltype(&ScAgent::CheckInitiationCondition), decltype(&TScAgent::CheckInitiationCondition)>::
              value;
    };

    struct GetInitiationCondition
    {
      static constexpr bool value =
          !std::is_same<decltype(&ScAgent::GetInitiationCondition), decltype(&TScAgent::GetInitiationCondition)>::value;
    };

    struct GetInitiationConditionTemplate
    {
    private:
      template <typename TBaseScAgent>
      struct OverrideGetInitiationConditionTemplateFrom
      {
        static constexpr bool value = !std::is_same<
            decltype(&TBaseScAgent::GetInitiationConditionTemplate),
            decltype(&TScAgent::GetInitiationConditionTemplate)>::value;
      };

    public:
      static constexpr bool value = std::is_base_of<ScActionAgent, TScAgent>::value
                                        ? OverrideGetInitiationConditionTemplateFrom<ScActionAgent>::value
                                        : OverrideGetInitiationConditionTemplateFrom<ScAgent>::value;
    };

    struct DoProgramWithEventArgument
    {
    private:
      template <typename U>
      static auto test(int)
          -> decltype(std::declval<U>().DoProgram(std::declval<typename TScAgent::TEventType const &>(), std::declval<ScAction &>()), std::true_type());

      template <typename>
      static std::false_type test(...);

    public:
      static bool const value = decltype(test<TScAgent>(0))::value;
    };

    struct DoProgramWithoutEventArgument
    {
    private:
      template <typename U>
      static auto test(int) -> decltype(std::declval<U>().DoProgram(std::declval<ScAction &>()), std::true_type());

      template <typename>
      static std::false_type test(...);

    public:
      static bool const value = decltype(test<TScAgent>(0))::value;
    };

    struct CheckResultCondition
    {
      static constexpr bool value =
          !std::is_same<decltype(&ScAgent::CheckResultCondition), decltype(&TScAgent::CheckResultCondition)>::value;
    };

    struct GetResultCondition
    {
      static constexpr bool value =
          !std::is_same<decltype(&ScAgent::GetResultCondition), decltype(&TScAgent::GetResultCondition)>::value;
    };

    struct GetResultConditionTemplate
    {
    private:
      template <typename TBaseScAgent>
      struct OverrideGetResultConditionTemplateFrom
      {
        static constexpr bool value = !std::is_same<
            decltype(&TBaseScAgent::GetResultConditionTemplate),
            decltype(&TScAgent::GetResultConditionTemplate)>::value;
      };

    public:
      static constexpr bool value = std::is_base_of<ScActionAgent, TScAgent>::value
                                        ? OverrideGetResultConditionTemplateFrom<ScActionAgent>::value
                                        : OverrideGetResultConditionTemplateFrom<ScAgent>::value;
    };
  };

  template <typename TScAgent>
  struct HasNoMoreThanOneOverride
  {
    // Each agent should have no more than one of three methods working with initiation condition.
    struct InitiationConditionMethod
    {
      static constexpr bool value =
          (HasOverride<TScAgent>::CheckInitiationCondition::value + HasOverride<TScAgent>::GetInitiationCondition::value
           + HasOverride<TScAgent>::GetInitiationConditionTemplate::value)
          <= 1;
    };

    // Each agent should have no more than one of three methods working with result condition.
    struct ResultConditionMethod
    {
      static constexpr bool value =
          (HasOverride<TScAgent>::CheckResultCondition::value + HasOverride<TScAgent>::GetInitiationCondition::value
           + HasOverride<TScAgent>::GetInitiationConditionTemplate::value)
          <= 1;
    };
  };

  template <typename TScAgent>
  struct HasOneOverride
  {
  public:
    struct DoProgramMethod
    {
      static bool const value = HasOverride<TScAgent>::DoProgramWithEventArgument::value
                                || HasOverride<TScAgent>::DoProgramWithoutEventArgument::value;
    };
  };

  friend class ScModule;
  friend class ScActionAgent;
  friend class ScAgentContext;

private:
  /*!
   * @brief Subscribes agent class to specified sc-events.
   *
   * If provided agent implementation `agentImplementationAddr` is valid then this method searches in knowledge base the
   * following constructions
   *
   * @code
   *             nrel_inclusion
   *                  |
   *                  |
   *                  |
   *                  \/
   * abstract_agent ======> agent_implementation
   *
   *   nrel_primary_initiation_condition
   *                  |
   *                  |
   *                  |  event_class
   *                  \/     ||
   * abstract_agent =======> ||
   *                         ||
   *                         \/
   *               event_subscription_element
   * @endcode
   *
   * and subscribes the agent of this class to sc-event `event_class` with subscription sc-element
   * `event_subscription_element`, else subscribes the agent of this class to sc-event
   * `TScEvent` with subscription sc-elements from `subscriptionAddrs`.
   *
   * @tparam TScAgent An agent class to be subscribed from the event.
   * @param context A sc-memory context used to subscribe agent class to specified sc-event.
   * @param agentImplementationAddr A sc-address of agent implementation specified in knowledge base for this agent
   * class.
   * @param subscriptionAddrs A list of sc-addresses of sc-elements to subscribe to.
   * @warning Specified agent class must be derived from class `ScAgent`.
   * @throws utils::ExceptionInvalidParams if any of the subscription addresses are invalid.
   * @throws utils::ExceptionInvalidState if the agent is already subscribed to the event.
   * @throws utils::ExceptionInvalidState if the agent implementation for this agent class is valid and is not included
   * in any abstract sc-agent.
   * @throws utils::ExceptionInvalidState if the agent implementation for this agent class is valid and the abstract
   * sc-agent for this agent class does not have a primary initiation condition.
   */
  template <class TScAgent, class... TScAddr>
  static _SC_EXTERN void Subscribe(
      ScMemoryContext * context,
      ScAddr const & agentImplementationAddr,
      TScAddr const &... subscriptionAddrs) noexcept(false);

  /*!
   * @brief Unsubscribes agent class from specified sc-events.
   *
   * If provided agent implementation `agentImplementationAddr` is valid then this method searches in knowledge base the
   * following constructions
   *
   * @code
   *             nrel_inclusion
   *                  |
   *                  |
   *                  |
   *                  \/
   * abstract_agent ======> agent_implementation
   *
   *   nrel_primary_initiation_condition
   *                  |
   *                  |
   *                  |  event_class
   *                  \/     ||
   * abstract_agent =======> ||
   *                         ||
   *                         \/
   *               event_subscription_element
   * @endcode
   *
   * and unsubscribes the agent of this class from sc-event `event_class` with subscription sc-element
   * `event_subscription_element`, else unsubscribes the agent of this class from sc-event
   * `TScEvent` with subscription sc-elements from `subscriptionAddrs`.
   *
   * @tparam TScAgent An agent class to be unsubscribed from the event.
   * @param context A sc-memory context used to unsubscribe agent class from specified sc-event.
   * @param agentImplementationAddr A sc-address of agent implementation specified in knowledge base for this agent
   * class.
   * @param subscriptionAddrs A list of sc-addresses of sc-elements to unsubscribe from.
   * @warning Specified agent class must be derived from class `ScAgent`.
   * @throws utils::ExceptionInvalidParams if any of the subscription addresses are invalid.
   * @throws utils::ExceptionInvalidState if the agent is not subscribed to the event.
   * @throws utils::ExceptionInvalidState if the agent implementation for this agent class is valid and is not included
   * in any abstract sc-agent.
   * @throws utils::ExceptionInvalidState if the agent implementation for this agent class is valid and the abstract
   * sc-agent for this agent class does not have a primary initiation condition.
   */
  template <class TScAgent, class... TScAddr>
  static _SC_EXTERN void Unsubscribe(
      ScMemoryContext * context,
      ScAddr const & agentImplementationAddr,
      TScAddr const &... subscriptionAddrs) noexcept(false);

protected:
  _SC_EXTERN explicit ScAgent() noexcept;

  /*!
   * @brief Gets the callback function for agent class.
   * @tparam TScAgent An agent class to be subscribed from the event.
   * @param agentImplementationAddr A sc-address of agent implementation specified in knowledge base for this agent
   * class.
   * @return A function that takes an sc-event and returns an sc-result.
   * @warning Specified agent class must be derived from class `ScAgent`.
   */
  template <class TScAgent>
  static _SC_EXTERN std::function<void(TScEvent const &)> GetCallback(ScAddr const & agentImplementationAddr) noexcept;

private:
  bool IsActionClassDeactivated() noexcept;

  template <class TScAgent>
  bool ValidateInitiationCondition(TScEvent const & event) noexcept;

  template <class TScAgent>
  bool ValidateResultCondition(TScEvent const & event, ScAction & action) noexcept;

  ScTemplate BuildCheckTemplate(TScEvent const & event, ScAddr const & checkTemplateAddr) noexcept;

  bool GenerateCheckTemplateParams(
      ScAddr const & checkTemplateAddr,
      ScAddr const & eventSubscriptionElementAddr,
      ScAddr const & otherElementAddr,
      size_t otherElementPosition,
      ScIterator5Ptr const eventTripleIterator,
      ScTemplateParams & checkTemplateParams) noexcept;
};

using ScBaseAgent = ScAgent<ScElementaryEvent>;

/*!
 * @class ScActionAgent
 * @brief A specialized agent class for handling sc-actions.
 *
 * This class extends ScAgent and provides methods for subscribing and unsubscribing
 * to sc-action events. You can inherit this class and implement your own agent class.
 *
 * @warning Derived classes must override the methods: `GetActionClass` and `DoProgram`.
 *
 * @code
 * // File my_agent.hpp:
 * #pragma once
 *
 * #include <sc-memory/sc_agent.hpp>
 *
 * class MyAgent : public ScActionAgent
 * {
 * public:
 *   ScAddr GetActionClass() const override;
 *
 *   ScResult DoProgram(ScAction & action) override;
 * };
 *
 * // File my_agent.cpp:
 * #include "my-module/agents/my_agent.hpp"
 * #include "my-module/keynodes/my_keynodes.hpp"
 *
 * ScAddr MyAgent::GetActionClass() const
 * {
 *   return MyKeynodes::my_agent_action;
 * }
 *
 * ScResult MyAgent::DoProgram(ScAction & action)
 * {
 *   auto const & [argAddr1, argAddr2] = action.GetArguments<2>();
 *   if (!argAddr1.IsValid() || !argAddr2.IsValid())
 *     return action.FinishWithError();
 *
 *   // Write agent logic here.
 *
 *   action.SetResult(resultAddr);
 *   return action.FinishSuccessfully();
 * }
 * @endcode
 */
class _SC_EXTERN ScActionAgent : public ScAgent<ScActionEvent>
{
  friend class ScAgentContext;

public:
  /*!
   * @brief Get sc-template that other sc-element of initiated sc-event belongs to action class that this agent class
   * performs.
   * @return ScTemplate of initiation condition of this agent class.
   */
  _SC_EXTERN ScTemplate GetInitiationConditionTemplate() const override;

protected:
  ScActionAgent() noexcept;
};

#include "sc_agent.tpp"
