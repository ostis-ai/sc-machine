/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory.hpp"

class ScAction;
class ScSet;
class ScStructure;
class ScKeynode;
class ScElementaryEvent;
class ScEventSubscription;
template <class TScEvent>
class ScElementaryEventSubscription;
class ScWaiter;
class ScActionInitiatedAgent;

/*!
 * @class ScAgentContext
 * @brief Represents the memory context for agents.
 *
 * ScAgentContext is derived from ScMemoryContext and provides functionality
 * for managing agent-specific operations, including event subscriptions,
 * actions, and structures within the sc-machine.
 */
class ScAgentContext : public ScMemoryContext
{
  template <class TScEvent, class TScContext>
  friend class ScAgentBase;
  friend class ScAction;

  SC_DISALLOW_COPY(ScAgentContext);

public:
  /*!
   * @brief Default constructor for ScAgentContext.
   */
  _SC_EXTERN explicit ScAgentContext() noexcept;

  /*!
   * @brief Constructor for ScAgentContext with a given memory context.
   * @param context Pointer to sc-memory context.
   */
  _SC_EXTERN explicit ScAgentContext(sc_memory_context * context) noexcept;

  /*!
   * @brief Move constructor for ScAgentContext.
   * @param other Other ScAgentContext to move from.
   */
  _SC_EXTERN ScAgentContext(ScAgentContext && other) noexcept;

  /*!
   * @brief Move assignment operator for ScAgentContext.
   * @param other Other ScAgentContext to move from.
   * @return Reference to the current ScAgentContext object.
   */
  _SC_EXTERN ScAgentContext & operator=(ScAgentContext && other) noexcept;

  /*!
   * @brief Creates elementary sc-event subscription for the specified sc-event class and subscription sc-element.
   *
   * This function allows the user to subscribe to events of a specified class,
   * using the provided subscription element address and a callback function
   * that is invoked when the event occurs.
   *
   * @param eventClassAddr An address of sc-event class to subscribe to.
   *                       This must be a valid sc-element of type `sc_event`.
   * @param subscriptionElementAddr An address of subscription sc-element, which must be a valid sc-element.
   * @param eventCallback An callback function that will be called when sc-event occurs.
   *                      It takes const reference to `ScElementaryEvent` object as a parameter.
   * @return A shared pointer to created `ScElementaryEventSubscription`.
   * @throws utils::ExceptionInvalidParams If the event class address or subscription element address is not valid,
   *         or if the event class does not belong to `sc_event`.
   */
  std::shared_ptr<ScElementaryEventSubscription<ScElementaryEvent>> CreateElementaryEventSubscription(
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr,
      std::function<void(ScElementaryEvent const &)> const & eventCallback) noexcept(false);

  /*!
   * @brief Creates elementary sc-event subscription for specified subscription sc-element and sc-event callback.
   * @tparam TScEvent A type of sc-event. It must be derived from ScElementaryEvent.
   * @param subscriptionElementAddr An address of subscription sc-element.
   * @param eventCallback A callback function that will be called when sc-event occurs. It takes a const reference to
   * TScEvent as a parameter.
   * @return A shared pointer to created `ScElementaryEventSubscription`.
   * @throws utils::ExceptionInvalidParams If subscription sc-element is not valid or if TScEvent is not derived from
   * ScElementaryEvent.
   */
  template <class TScEvent>
  _SC_EXTERN std::shared_ptr<ScElementaryEventSubscription<TScEvent>> CreateElementaryEventSubscription(
      ScAddr const & subscriptionElementAddr,
      std::function<void(TScEvent const &)> const & eventCallback) noexcept(false);

  /*!
   * @brief Creates sc-event wait for specified event class and subscription sc-element.
   *
   * This function allows user to create a wait mechanism that will be triggered
   * when sc-event of specified class occurs. An optional callback can be
   * provided to execute when the wait starts.
   *
   * @param eventClassAddr An address of sc-event class to wait for.
   *                       This must be valid sc-element belongs to `sc_event` class.
   * @param subscriptionElementAddr An address of subscription sc-element, which must be valid sc-element.
   * @param initiateCallback An optional callback function that will be called when the wait starts.
   *                         It takes no parameters. Defaults to an empty function if not provided.
   * @return A shared pointer to created `ScWaiter`.
   * @throws utils::ExceptionInvalidParams If sc-event class or subscription sc-element is not valid.
   */
  _SC_EXTERN std::shared_ptr<ScWaiter> CreateEventWaiter(
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr,
      std::function<void(void)> const & initiateCallback = {}) noexcept(false);

  /*!
   * @brief Creates sc-event wait for the specified subscription sc-element and optional initiate callback.
   * @tparam TScEvent A type of sc-event. It must be derived from ScElementaryEvent.
   * @param subscriptionElementAddr An address of subscription sc-element.
   * @param initiateCallback An optional callback function that will be called when the wait starts. It takes no
   * parameters.
   * @return A shared pointer to created `ScWaiter`.
   * @throws utils::ExceptionInvalidParams If subscription sc-element is not valid or if TScEvent is not derived from
   * ScElementaryEvent.
   */
  template <class TScEvent>
  _SC_EXTERN std::shared_ptr<ScWaiter> CreateEventWaiter(
      ScAddr const & subscriptionElementAddr,
      std::function<void(void)> const & initiateCallback = {}) noexcept(false);

  /*!
   * @brief Creates sc-event wait with condition for the specified sc-event class and subscription sc-element.
   *
   * This function allows user to create a wait mechanism that will be triggered
   * when sc-event of specified class occurs, provided that the specified condition
   * is met. An optional callback can be provided to execute when the wait starts.
   *
   * @param eventClassAddr An address of sc-event class to wait for.
   *                       This must be valid sc-element belongs to `sc_event` class.
   * @param subscriptionElementAddr An address of the subscription element, which must be valid sc-element.
   * @param initiateCallback A callback function that will be called when the wait starts.
   *                         It takes no parameters.
   * @param checkCallback A callback function that will be called to check if condition is met.
   *                      It takes a const reference to `ScElementaryEvent` object as a parameter and
   *                      returns an `bool`.
   * @return A shared pointer to created `ScWaiter`.
   * @throws utils::ExceptionInvalidParams If sc-event class address or subscription sc-element is not valid.
   */
  std::shared_ptr<ScWaiter> CreateConditionWaiter(
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr,
      std::function<void(void)> const & initiateCallback,
      std::function<bool(ScElementaryEvent const &)> const & checkCallback) noexcept(false);

  /*!
   * @brief Creates sc-event wait with condition for the specified subscription sc-element, initiate callback, and check
   * callback.
   * @tparam TScEvent A type of sc-event. It must be derived from ScElementaryEvent.
   * @param subscriptionElementAddr An address of subscription sc-element.
   * @param initiateCallback A callback function that will be called when the wait starts. It takes no parameters.
   * @param checkCallback A callback function that will be called to check if the condition is met. It takes a const
   * reference to TScEvent as a parameter and returns an bool.
   * @return A shared pointer to created `ScWaiter`.
   * @throws utils::ExceptionInvalidParams If subscription sc-element is not valid or if TScEvent is not derived from
   * ScElementaryEvent.
   */
  template <class TScEvent>
  std::shared_ptr<ScWaiter> CreateConditionWaiter(
      ScAddr const & subscriptionElementAddr,
      std::function<void(void)> const & initiateCallback,
      std::function<bool(TScEvent const &)> const & checkCallback) noexcept(false);

  /*!
   * @brief Creates sc-event wait with condition for the specified sc-event class and subscription sc-element.
   *
   * This function allows user to create a wait mechanism that will be triggered
   * when sc-event of the specified class occurs, provided that the specified condition
   * is met. This version does not include an initiate callback.
   *
   * @param eventClassAddr An address of sc-event class to wait for.
   *                       This must be a valid sc-element of type `sc_event`.
   * @param subscriptionElementAddr An address of subscription sc-element, which must be a valid sc-element.
   * @param checkCallback A callback function that will be called to check if condition is met.
   *                      It takes a const reference to `ScElementaryEvent` object as a parameter and
   *                      returns an `bool`.
   * @return A shared pointer to created `ScWaiter`.
   * @throws utils::ExceptionInvalidParams If sc-event class or subscription sc-element is not valid.
   */
  std::shared_ptr<ScWaiter> CreateConditionWaiter(
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr,
      std::function<bool(ScElementaryEvent const &)> const & checkCallback) noexcept(false);

  /*!
   * @brief Creates sc-event wait with condition for the specified subscription sc-element and check callback.
   * @tparam TScEvent A type of sc-event. It must be derived from ScElementaryEvent.
   * @param subscriptionElementAddr A address of subscription sc-element.
   * @param checkCallback A callback function that will be called to check if the condition is met. It takes a const
   * reference to TScEvent as a parameter and returns an bool.
   * @return A shared pointer to created `ScWaiter`.
   * @throws utils::ExceptionInvalidParams If subscription sc-element is not valid or if TScEvent is not derived from
   * ScElementaryEvent.
   */
  template <class TScEvent>
  std::shared_ptr<ScWaiter> CreateConditionWaiter(
      ScAddr const & subscriptionElementAddr,
      std::function<bool(TScEvent const &)> const & checkCallback) noexcept(false);

  /*!
   * @brief Subscribes agent class to specified sc-events.
   * @tparam TScAgent An agent class to be subscribed to the event.
   * @param subscriptionAddrs A list of sc-addresses of sc-elements to subscribe to.
   * @warning Specified agent class must be derived from class `ScAgent`.
   * @throws utils::ExceptionInvalidParams if any of the subscription addresses are invalid.
   * @throws utils::ExceptionInvalidState if the agent is already subscribed to the event.
   */
  template <class TScAgent, class... TScAddr>
  _SC_EXTERN typename std::enable_if<!std::is_base_of<ScActionInitiatedAgent, TScAgent>::value>::type SubscribeAgent(
      TScAddr const &... subscriptionAddrs) noexcept(false);

  /*!
   * @brief Subscribes agent class to sc-event of adding outgoing sc-arc from `action_initiated` to some formed
   * sc-action.
   * @tparam TScAgent An agent class to be subscribed to the event.
   * @warning Specified agent class must be derived from class `ScAgentAction`.
   * @throws utils::ExceptionInvalidParams if any of the subscription addresses are invalid.
   * @throws utils::ExceptionInvalidState if the agent is already subscribed to the event.
   */
  template <class TScAgent>
  _SC_EXTERN typename std::enable_if<std::is_base_of<ScActionInitiatedAgent, TScAgent>::value>::type
  SubscribeAgent() noexcept(false);

  /*!
   * @brief Unsubscribes agent class from specified sc-events.
   * @tparam TScAgent An agent class to be unsubscribed from the event.
   * @param subscriptionAddrs A list of sc-addresses of sc-elements to unsubscribe from.
   * @warning Specified agent class must be derived from class `ScAgent`.
   *  @throws utils::ExceptionInvalidParams if any of the subscription addresses are invalid.
   * @throws utils::ExceptionInvalidState if the agent is not subscribed to the event.
   */
  template <class TScAgent, class... TScAddr>
  _SC_EXTERN typename std::enable_if<!std::is_base_of<ScActionInitiatedAgent, TScAgent>::value>::type UnsubscribeAgent(
      TScAddr const &... subscriptionAddrs) noexcept(false);

  /*!
   * @brief Unsubscribes agent class from sc-event of adding outgoing sc-arc from `action_initiated` to some formed
   * sc-action.
   * @tparam TScAgent An agent class to be unsubscribed from the event.
   * @warning Specified agent class must be derived from class `ScAgentAction`.
   * @throws utils::ExceptionInvalidParams if any of the subscription addresses are invalid.
   * @throws utils::ExceptionInvalidState if the agent is not subscribed to the event.
   */
  template <class TScAgent>
  _SC_EXTERN typename std::enable_if<std::is_base_of<ScActionInitiatedAgent, TScAgent>::value>::type
  UnsubscribeAgent() noexcept(false);

  /*!
   * @brief Loads specification of agent implementation and subscribes agent class.
   * @tparam TScAgent An agent class to be subscribed to the event.
   * @param agentImplementationAddr A sc-address of agent implementation specified in knowledge base for this agent
   * class.
   * @throws utils::ExceptionInvalidParams if any of the subscription addresses are invalid.
   * @throws utils::ExceptionInvalidState if the agent is already subscribed to the event.
   * @throws utils::ExceptionInvalidState if the agent implementation for this agent class is valid and is not included
   * in any abstract sc-agent.
   * @throws utils::ExceptionInvalidState if the agent implementation for this agent class is valid and the abstract
   * sc-agent for this agent class does not have a primary initiation condition.
   */
  template <class TScAgent>
  _SC_EXTERN void LoadAndSubscribeAgent(ScAddr const & agentImplementationAddr) noexcept(false);

  /*!
   * @brief Remove from program specification of agent and unsubscribes agent class.
   * @tparam TScAgent An agent class to be unsubscribed from the event.
   * @param agentImplementationAddr A sc-address of agent implementation specified in knowledge base for this agent
   * class.
   * @throws utils::ExceptionInvalidParams if any of the subscription addresses are invalid.
   * @throws utils::ExceptionInvalidState if the agent is not subscribed to the event.
   * @throws utils::ExceptionInvalidState if the agent implementation for this agent class is valid and is not included
   * in any abstract sc-agent.
   * @throws utils::ExceptionInvalidState if the agent implementation for this agent class is valid and the abstract
   * sc-agent for this agent class does not have a primary initiation condition.
   */
  template <class TScAgent>
  _SC_EXTERN void DestroyAndUnsubscribeAgent(ScAddr const & agentImplementationAddr) noexcept(false);

  /*!
   * @brief Creates an action with a given action class.
   * @param actionClassAddr An address of the action class.
   * @return ScAction object.
   */
  _SC_EXTERN ScAction CreateAction(ScAddr const & actionClassAddr) noexcept(false);

  /*!
   * @brief Create ScAction object for provided action.
   * @param actionAddr Address of the action.
   * @return ScAction object.
   */
  _SC_EXTERN ScAction ConvertToAction(ScAddr const & actionAddr) noexcept(false);

  /*!
   * @brief Creates a set.
   * @return ScSet object.
   */
  _SC_EXTERN ScSet CreateSet();

  /*!
   * @brief Creates ScSet object for provided set.
   * @param setAddr An address of the set.
   * @return ScSet object.
   */
  _SC_EXTERN ScSet ConvertToSet(ScAddr const & setAddr) noexcept(false);

  /*!
   * @brief Creates a structure.
   * @return ScStructure object.
   */
  _SC_EXTERN ScStructure CreateStructure();

  /*!
   * @brief Creates ScStructure object for provided set.
   * @param structureAddr An address of the set.
   * @return ScStructure object.
   */
  _SC_EXTERN ScStructure ConvertToStructure(ScAddr const & structureAddr) noexcept(false);

protected:
  /*!
   * @brief Constructor for ScAgentContext with a given user.
   * @param userAddr Address of the user.
   */
  _SC_EXTERN explicit ScAgentContext(ScAddr const & userAddr) noexcept;
};

#include "sc_agent_context.tpp"
