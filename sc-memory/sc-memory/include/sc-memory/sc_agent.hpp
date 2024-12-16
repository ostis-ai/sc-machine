/*
 * This source file is part of an OSTIS project. For the laTest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "sc_agent_context.hpp"

#include "utils/sc_logger.hpp"

//! Log functions to be used in agent class methods.
#define SC_AGENT_LOG_DEBUG(__message__) m_logger.Debug(__message__)
#define SC_AGENT_LOG_INFO(__message__) m_logger.Info(__message__)
#define SC_AGENT_LOG_WARNING(__message__) m_logger.Warning(__message__)
#define SC_AGENT_LOG_ERROR(__message__) m_logger.Error(__message__)

template <class TScEvent>
class ScElementaryEventSubscription;
class ScAction;
class ScResult;
class ScEvent;

/*!
 * @class ScAgent
 * @brief An abstract base class for agents.
 *
 * This class provides a base implementation for agents, offering methods for initialization, shutdown, and handling
 * events.
 *
 * @tparam TScEvent The type of sc-event this agent handles.
 * @tparam TScContext The type of sc-memory context that used by agent.
 */
template <class TScEvent, class TScContext = ScAgentContext>
class _SC_EXTERN ScAgent : public ScObject
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");
  static_assert(
      std::is_base_of<ScAgentContext, TScContext>::value,
      "TScContext type must be derived from ScAgentContext type.");
  static_assert(std::is_move_constructible<TScContext>::value, "TScContext type must be movable constructible.");

  template <class TScAgent>
  friend class ScAgentBuilder;
  template <class TScAgent>
  friend class ScAgentManager;

  using TEventType = TScEvent;
  using TContextType = TScContext;

public:
  _SC_EXTERN ~ScAgent() noexcept override;

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
   * @throws utils::ExceptionItemNotFound if the agent implementation for this agent class is not included in any
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
   * @throws utils::ExceptionItemNotFound if the abstract sc-agent for this agent class does not have a primary
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
   * @throws utils::ExceptionItemNotFound if the abstract sc-agent for this agent class does not have a primary
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
   * @throws utils::ExceptionItemNotFound if the abstract sc-agent for this agent class does not have an action class.
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
   * @throws utils::ExceptionItemNotFound if the abstract sc-agent for this agent class does not have an initiation
   * condition.
   */
  _SC_EXTERN virtual ScAddr GetInitiationCondition() const noexcept(false);

  /*!
   * @brief Gets initiation condition template represented in program.
   * @param event A sc-event that triggered the agent.
   * @return A sc-template of initiation condition.
   */
  _SC_EXTERN virtual ScTemplate GetInitiationConditionTemplate(TScEvent const & event) const;

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
   * @throws utils::ExceptionItemNotFound if the abstract sc-agent for this agent class does not have an result
   * condition.
   */
  _SC_EXTERN virtual ScAddr GetResultCondition() const noexcept(false);

  /*!
   * @brief Gets result condition template represented in program.
   * @param event An initiated sc-event to which the agent reacted.
   * @param action A sc-action that the agent performs.
   * @return A sc-template of result condition.
   */
  _SC_EXTERN virtual ScTemplate GetResultConditionTemplate(TScEvent const & event, ScAction & action) const;

protected:
  mutable TScContext m_context;
  mutable utils::ScLogger m_logger;
  ScAddr m_agentImplementationAddr;

  _SC_EXTERN ScAgent() noexcept;

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

  //! Checks that agent action class belongs to `action_deactivated`.
  _SC_EXTERN bool IsActionClassDeactivated() noexcept;

  /*!
   * @brief Checks if the agent may be specified in knowledge base.
   * @return true if the agent has implementation in knowledge base, otherwise true.
   */
  _SC_EXTERN bool MayBeSpecified() const noexcept;

  /*!
   * @brief Validates the initiation condition for a given event.
   *
   * This method checks if the initiation condition for the agent can be validated
   * based on the provided event. It first checks for any overridden initiation
   * condition checks. If none are found, it builds the initiation condition template
   * based on the event and checks if it is empty. If the template is not empty,
   * it performs a search for the template.
   *
   * @param event An event to validate against.
   * @return true if the initiation condition is valid; false otherwise.
   */
  template <class TScAgent, typename HasOverride>
  _SC_EXTERN bool ValidateInitiationCondition(TScEvent const & event) noexcept(false);

  /*!
   * @brief Validates the result condition for a given event and action.
   *
   * This method checks if the result condition for the agent can be validated
   * based on the provided event and action. It first checks for any overridden
   * result condition checks. If none are found, it builds the result condition
   * template and checks if it is empty. If the template is not empty, it performs
   * a search for the template against the action's result.
   *
   * @param event An event to validate against.
   * @param action An action associated with the event.
   * @return true if the result condition is valid; false otherwise.
   */
  template <class TScAgent, typename HasOverride>
  _SC_EXTERN bool ValidateResultCondition(TScEvent const & event, ScAction & action) noexcept(false);

  /*!
   * @brief Builds the initiation condition template based on the event and template address.
   *
   * This method constructs the initiation condition template using the provided
   * event and an address of the initiation condition template. It uses
   * iterators to find relevant sc-elements and generates necessary template
   * parameters for validation.
   *
   * @param event An event to build the template for.
   * @param initiationConditionTemplateAddr An address of the initiation condition template.
   * @return A ScTemplate object representing a initiation condition template.
   */
  _SC_EXTERN ScTemplate
  BuildInitiationConditionTemplate(TScEvent const & event, ScAddr const & initiationConditionTemplateAddr) noexcept;

  /*!
   * @brief Builds the result condition template based on the event and template address.
   *
   * This method constructs the result condition template using the provided
   * event and the address of the result condition template.
   *
   * @param event An event to build the template for.
   * @param resultConditionTemplateAddr An address of the result condition template.
   * @return A ScTemplate object representing a result condition template.
   */
  _SC_EXTERN ScTemplate
  BuildResultConditionTemplate(TScEvent const & event, ScAddr const & resultConditionTemplateAddr) noexcept;

  /*!
   * @brief Generates template parameters for checking the initiation condition.
   *
   * This method generates parameters for validating the initiation
   * condition template based on the event and its associated elements. It checks
   * the types of elements involved and ensures they are substitutable according
   * to the template's expectations.
   *
   * @param initiationConditionTemplateAddr An address of the initiation condition template.
   * @param event An event to generate parameters for.
   * @param otherElementPosition A position of the other element in the template.
   * @param eventTripleIterator An iterator for the event's triples.
   * @param checkTemplateParams Parameters to fill with found values.
   * @return true if the parameters were successfully generated; false otherwise.
   */
  _SC_EXTERN bool GenerateCheckTemplateParams(
      ScAddr const & checkTemplateAddr,
      TScEvent const & event,
      size_t otherElementPosition,
      ScIterator5Ptr const & eventTripleIterator,
      ScTemplateParams & checkTemplateParams) noexcept;
};

using ScElementaryEventAgent = ScAgent<ScElementaryEvent>;

/*!
 * @class ScActionInitiatedAgent
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
 * class MyAgent : public ScActionInitiatedAgent
 * {
 * public:
 *   ScAddr GetActionClass() const override;
 *
 *   ScResult DoProgram(ScAction & action) override;
 * };
 *
 * // File my_agent.cpp:
 * #include "my-module/agent/my_agent.hpp"
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
 *   if (!m_context.IsElement(argAddr1) || !m_context.IsElement(argAddr2))
 *     return action.FinishWithError();
 *
 *   // Write agent logic here.
 *
 *   action.SetResult(resultAddr);
 *   return action.FinishSuccessfully();
 * }
 * @endcode
 */
class _SC_EXTERN ScActionInitiatedAgent : public ScAgent<ScActionInitiatedEvent>
{
  friend class ScAgentContext;

public:
  //! Returns `ScKeynodes::action_initiated`.
  _SC_EXTERN ScAddr GetEventSubscriptionElement() const override;

  /*!
   * @brief Get sc-template that other sc-element of initiated sc-event belongs to action class that this agent class
   * performs.
   * @return ScTemplate of initiation condition of this agent class.
   */
  _SC_EXTERN ScTemplate GetInitiationConditionTemplate(ScActionInitiatedEvent const & event) const override;

protected:
  ScActionInitiatedAgent() noexcept;
};

#include "_template/sc_agent.tpp"
