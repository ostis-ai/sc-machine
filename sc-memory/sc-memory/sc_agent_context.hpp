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
class ScWait;

/*!
 * @class ScAgentContext
 * @brief Represents context for an sc-agent. ScAgentContext is derived from ScMemoryContext.
 */
class ScAgentContext : public ScMemoryContext
{
  template <class TScEvent>
  friend class ScAgentAbstract;
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
   * @brief Creates an elementary event subscription for the specified event class and subscription element.
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
  std::shared_ptr<ScElementaryEventSubscription<ScElementaryEvent>> CreateEventSubscription(
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr,
      std::function<void(ScElementaryEvent const &)> const & eventCallback) const;

  /*!
   * @brief Creates sc-event subscription for the specified subscription address and event callback.
   * @tparam TScEvent A type of sc-event. It must be derived from ScElementaryEvent.
   * @param subscriptionElementAddr An address of subscription sc-element.
   * @param eventCallback The callback function that will be called when sc-event occurs. It takes a const reference to
   * TScEvent as a parameter.
   * @return A shared pointer to created `ScElementaryEventSubscription`.
   * @throws utils::ExceptionInvalidParams If subscription sc-element is not valid or if TScEvent is not derived from
   * ScElementaryEvent.
   */
  template <class TScEvent>
  _SC_EXTERN std::shared_ptr<ScElementaryEventSubscription<TScEvent>> CreateEventSubscription(
      ScAddr const & subscriptionElementAddr,
      std::function<void(TScEvent const &)> const & eventCallback) const;

  /*!
   * @brief Creates sc-event wait for specified event class and subscription sc-element.
   *
   * This function allows user to create a wait mechanism that will trigger
   * when sc-event of specified class occurs. An optional callback can be
   * provided to execute when the wait starts.
   *
   * @param eventClassAddr An address of sc-event class to wait for.
   *                       This must be valid sc-element of type `sc_event`.
   * @param subscriptionElementAddr An address of subscription sc-element, which must be valid sc-element.
   * @param initiateCallback An optional callback function that will be called when the wait starts.
   *                         It takes no parameters. Defaults to an empty function if not provided.
   * @return A shared pointer to created `ScWait`.
   * @throws utils::ExceptionInvalidParams If sc-event class or subscription sc-element is not valid.
   */
  _SC_EXTERN std::shared_ptr<ScWait> CreateEventWait(
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr,
      std::function<void(void)> const & initiateCallback = {}) const;

  /*!
   * @brief Creates sc-event wait for the specified subscription address and optional initiate callback.
   * @tparam TScEvent A type of sc-event. It must be derived from ScElementaryEvent.
   * @param subscriptionElementAddr An address of subscription sc-element.
   * @param initiateCallback An optional callback function that will be called when the wait starts. It takes no
   * parameters.
   * @return A shared pointer to created `ScWait`.
   * @throws utils::ExceptionInvalidParams If subscription sc-element is not valid or if TScEvent is not derived from
   * ScElementaryEvent.
   */
  template <class TScEvent>
  _SC_EXTERN std::shared_ptr<ScWait> CreateEventWait(
      ScAddr const & subscriptionElementAddr,
      std::function<void(void)> const & initiateCallback = {}) const;

  /*!
   * @brief Creates sc-event wait with condition for the specified sc-event class and subscription sc-element.
   *
   * This function allows user to create a wait mechanism that will trigger
   * when sc-event of specified class occurs, provided that the specified condition
   * is met. An optional callback can be provided to execute when the wait starts.
   *
   * @param eventClassAddr An address of sc-event class to wait for.
   *                       This must be valid sc-element of type `sc_event`.
   * @param subscriptionElementAddr An address of the subscription element, which must be valid sc-element.
   * @param initiateCallback A callback function that will be called when the wait starts.
   *                         It takes no parameters.
   * @param checkCallback A callback function that will be called to check if condition is met.
   *                      It takes a const reference to `ScElementaryEvent` object as a parameter and
   *                      returns an `sc_bool`.
   * @return A shared pointer to created `ScWait`.
   * @throws utils::ExceptionInvalidParams If sc-event class address or subscription sc-element is not valid.
   */
  std::shared_ptr<ScWait> CreateEventWaitWithCondition(
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr,
      std::function<void(void)> const & initiateCallback,
      std::function<sc_bool(ScElementaryEvent const &)> const & checkCallback) const;

  /*!
   * @brief Creates sc-event wait with condition for specified subscription sc-element, initiate callback, and check
   * callback.
   * @tparam TScEvent A type of sc-event. It must be derived from ScElementaryEvent.
   * @param subscriptionElementAddr An address of subscription sc-element.
   * @param initiateCallback A callback function that will be called when the wait starts. It takes no parameters.
   * @param checkCallback A callback function that will be called to check if the condition is met. It takes a const
   * reference to TScEvent as a parameter and returns an sc_bool.
   * @return A shared pointer to created `ScWait`.
   * @throws utils::ExceptionInvalidParams If subscription sc-element is not valid or if TScEvent is not derived from
   * ScElementaryEvent.
   */
  template <class TScEvent>
  std::shared_ptr<ScWait> CreateEventWaitWithCondition(
      ScAddr const & subscriptionElementAddr,
      std::function<void(void)> const & initiateCallback,
      std::function<sc_bool(TScEvent const &)> const & checkCallback) const;

  /*!
   * @brief Creates sc-event wait with condition for the specified sc-event class and subscription sc-element.
   *
   * This function allows user to create a wait mechanism that will trigger
   * when sc-event of the specified class occurs, provided that the specified condition
   * is met. This version does not include an initiate callback.
   *
   * @param eventClassAddr An address of sc-event class to wait for.
   *                       This must be a valid sc-element of type `sc_event`.
   * @param subscriptionElementAddr An address of subscription sc-element, which must be a valid sc-element.
   * @param checkCallback A callback function that will be called to check if condition is met.
   *                      It takes a const reference to `ScElementaryEvent` object as a parameter and
   *                      returns an `sc_bool`.
   * @return A shared pointer to created `ScWait`.
   * @throws utils::ExceptionInvalidParams If sc-event class or subscription sc-element is not valid.
   */
  std::shared_ptr<ScWait> CreateEventWaitWithCondition(
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr,
      std::function<sc_bool(ScElementaryEvent const &)> const & checkCallback) const;

  /*!
   * @brief Creates sc-event wait with condition for specified subscription sc-element and check callback.
   * @tparam TScEvent A type of sc-event. It must be derived from ScElementaryEvent.
   * @param subscriptionElementAddr A address of subscription sc-element.
   * @param checkCallback A callback function that will be called to check if the condition is met. It takes a const
   * reference to TScEvent as a parameter and returns an sc_bool.
   * @return A shared pointer to created `ScWait`.
   * @throws utils::ExceptionInvalidParams If subscription sc-element is not valid or if TScEvent is not derived from
   * ScElementaryEvent.
   */
  template <class TScEvent>
  std::shared_ptr<ScWait> CreateEventWaitWithCondition(
      ScAddr const & subscriptionElementAddr,
      std::function<sc_bool(TScEvent const &)> const & checkCallback) const;

  /*!
   * @brief Creates an action with a given action class.
   * @param actionClassAddr Address of the action class.
   * @return ScAction object.
   */
  _SC_EXTERN ScAction CreateAction(ScAddr const & actionClassAddr);

  /*!
   * @brief Create ScAction object for provided action.
   * @param actionAddr Address of the action.
   * @return ScAction object.
   */
  _SC_EXTERN ScAction UseAction(ScAddr const & actionAddr);

  /*!
   * @brief Creates a set.
   * @return ScSet object.
   */
  _SC_EXTERN ScSet CreateSet();

  /*!
   * @brief Create ScSet object for provided set.
   * @param setAddr Address of the set.
   * @return ScSet object.
   */
  _SC_EXTERN ScSet UseSet(ScAddr const & setAddr);

  /*!
   * @brief Creates a structure.
   * @return ScStructure object.
   */
  _SC_EXTERN ScStructure CreateStructure();

  /*!
   * @brief Create ScStructure object for provided set.
   * @param structureAddr Address of the set.
   * @return ScStructure object.
   */
  _SC_EXTERN ScStructure UseStructure(ScAddr const & structureAddr);

protected:
  /*!
   * @brief Constructor for ScAgentContext with a given user.
   * @param userAddr Address of the user.
   */
  _SC_EXTERN explicit ScAgentContext(ScAddr const & userAddr) noexcept;
};

#include "sc_agent_context.tpp"
