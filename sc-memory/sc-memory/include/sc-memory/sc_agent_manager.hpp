/*
 * This source file is part of an OSTIS project. For the laTest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <optional>

#include "sc_object.hpp"

#include "utils/sc_log.hpp"

template <class TScEvent>
class ScElementaryEventSubscription;
class ScAction;
class ScResult;
template <class TScEvent, class TScContext>
class ScAgent;

/*!
 * @class ScAgentManager
 * @brief A class for agents that can subscribe them to any elementary sc-events.
 *
 * This class provides functionality for subscribing/unsubscribing agents to sc-events and implements flow of performing
 * actions by agents.
 */
template <class TScAgent>
class _SC_EXTERN ScAgentManager : public ScObject
{
  using TScEvent = typename TScAgent::TEventType;
  using TScContext = typename TScAgent::TContextType;

  /*!
   * @brief Checks if the given agent type TScAgentType overrides certain methods.
   *
   * This struct contains nested structs that check if the agent type overrides specific methods related to initiation
   * condition, result condition, and DoProgram.
   *
   * @tparam TScAgentType The agent type to check for overridden methods.
   */
  template <class TScAgentType>
  struct HasOverride
  {
    //! Checks if the agent type overrides the GetEventClass method.
    struct GetEventClass
    {
      static constexpr bool value = !std::is_same<
          decltype(&ScAgent<TScEvent, TScContext>::GetEventClass),
          decltype(&TScAgentType::GetEventClass)>::value;
    };

    //! Checks if the agent type overrides the GetEventSubscriptionElement method.
    struct GetEventSubscriptionElement
    {
      static constexpr bool value = !std::is_same<
          decltype(&ScAgent<TScEvent, TScContext>::GetEventSubscriptionElement),
          decltype(&TScAgentType::GetEventSubscriptionElement)>::value;
    };

    //! Checks if the agent type overrides the CheckInitiationCondition method.
    struct CheckInitiationCondition
    {
      static constexpr bool value = !std::is_same<
          decltype(&ScAgent<TScEvent, TScContext>::CheckInitiationCondition),
          decltype(&TScAgentType::CheckInitiationCondition)>::value;
    };

    //! Checks if the agent type overrides the GetInitiationCondition method.
    struct GetInitiationCondition
    {
      static constexpr bool value = !std::is_same<
          decltype(&ScAgent<TScEvent, TScContext>::GetInitiationCondition),
          decltype(&TScAgentType::GetInitiationCondition)>::value;
    };

    //! Checks if the agent type overrides the GetInitiationConditionTemplate method.
    struct GetInitiationConditionTemplate
    {
    private:
      template <typename TBaseScAgent>
      struct OverrideGetInitiationConditionTemplateFrom
      {
        static constexpr bool value = !std::is_same<
            decltype(&TBaseScAgent::GetInitiationConditionTemplate),
            decltype(&TScAgentType::GetInitiationConditionTemplate)>::value;
      };

    public:
      static constexpr bool value =
          std::is_base_of<ScActionInitiatedAgent, TScAgentType>::value
              ? OverrideGetInitiationConditionTemplateFrom<ScActionInitiatedAgent>::value
              : OverrideGetInitiationConditionTemplateFrom<ScAgent<TScEvent, TScContext>>::value;
    };

    //! Checks if the agent type overrides the DoProgram(TScEvent const &, ScAction &) method.
    struct DoProgramWithEventArgument
    {
    private:
      template <typename U>
      static auto Test(int)
          -> decltype(std::declval<U>().DoProgram(std::declval<typename U::TEventType const &>(), std::declval<ScAction &>()), std::true_type());

      template <typename>
      static std::false_type Test(...);

    public:
      static bool const value = decltype(Test<TScAgentType>(0))::value;
    };

    //! Checks if the agent type overrides the DoProgram(ScAction &) method.
    struct DoProgramWithoutEventArgument
    {
    private:
      template <typename U>
      static auto Test(int) -> decltype(std::declval<U>().DoProgram(std::declval<ScAction &>()), std::true_type());

      template <typename>
      static std::false_type Test(...);

    public:
      static bool const value = decltype(Test<TScAgentType>(0))::value;
    };

    //! Checks if the agent type overrides the CheckResultCondition method.
    struct CheckResultCondition
    {
      static constexpr bool value = !std::is_same<
          decltype(&ScAgent<TScEvent, TScContext>::CheckResultCondition),
          decltype(&TScAgentType::CheckResultCondition)>::value;
    };

    //! Checks if the agent type overrides the GetResultCondition method.
    struct GetResultCondition
    {
      static constexpr bool value = !std::is_same<
          decltype(&ScAgent<TScEvent, TScContext>::GetResultCondition),
          decltype(&TScAgentType::GetResultCondition)>::value;
    };

    //! Checks if the agent type overrides the GetResultConditionTemplate method.
    struct GetResultConditionTemplate
    {
    private:
      template <typename TBaseScAgent>
      struct OverrideGetResultConditionTemplateFrom
      {
        static constexpr bool value = !std::is_same<
            decltype(&TBaseScAgent::GetResultConditionTemplate),
            decltype(&TScAgentType::GetResultConditionTemplate)>::value;
      };

    public:
      static constexpr bool value = std::is_base_of<ScActionInitiatedAgent, TScAgentType>::value
                                        ? OverrideGetResultConditionTemplateFrom<ScActionInitiatedAgent>::value
                                        : OverrideGetResultConditionTemplateFrom<ScAgent<TScEvent, TScContext>>::value;
    };
  };

  /*!
   * @brief Ensures that the agent type has no more than one override for initiation condition and result condition
   * methods.
   *
   * This struct contains nested structs that check if the agent type has no more than one override for the methods
   * related to initiation condition and result condition.
   *
   * @tparam TScAgentType The agent type to check for overridden methods.
   */
  template <class TScAgentType>
  struct HasNoMoreThanOneOverride
  {
    // Each agent should have no more than one of three methods working with initiation condition.
    struct InitiationConditionMethod
    {
      static constexpr bool value = (HasOverride<TScAgentType>::CheckInitiationCondition::value
                                     + HasOverride<TScAgentType>::GetInitiationCondition::value
                                     + HasOverride<TScAgentType>::GetInitiationConditionTemplate::value)
                                    <= 1;
    };

    // Each agent should have no more than one of three methods working with result condition.
    struct ResultConditionMethod
    {
      static constexpr bool value =
          (HasOverride<TScAgentType>::CheckResultCondition::value + HasOverride<TScAgentType>::GetResultCondition::value
           + HasOverride<TScAgentType>::GetResultConditionTemplate::value)
          <= 1;
    };
  };

  /*!
   * @brief Checks if the agent type has overridden the DoProgram method.
   *
   * This struct contains a nested struct that checks if the agent type has overridden either the
   * DoProgramWithEventArgument or DoProgramWithoutEventArgument method.
   *
   * @tparam TScAgentType The agent type to check for overridden DoProgram method.
   */
  template <class TScAgentType>
  struct HasOneOverride
  {
  public:
    struct DoProgramMethod
    {
      static bool const value = HasOverride<TScAgentType>::DoProgramWithEventArgument::value
                                || HasOverride<TScAgentType>::DoProgramWithoutEventArgument::value;
    };
  };

  friend class ScModule;
  friend class ScActionInitiatedAgent;
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
   * @tparam TScAgent An agent class to be subscribed to the event.
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
  template <class... TScAddr>
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
  template <class... TScAddr>
  static _SC_EXTERN void Unsubscribe(
      ScMemoryContext * context,
      ScAddr const & agentImplementationAddr,
      TScAddr const &... subscriptionAddrs) noexcept(false);

protected:
  using ScSubscriptions = ScAddrToValueUnorderedMap<ScEventSubscription *>;
  using ScAgentImplementationsToSubscriptions = ScAddrToValueUnorderedMap<ScSubscriptions>;
  using ScAgentClassesToAgentImplementationSubscriptions =
      std::unordered_map<std::string, ScAgentImplementationsToSubscriptions>;
  //! Map to store agent classes to their corresponding agent implementation subscriptions.
  static inline ScAgentClassesToAgentImplementationSubscriptions m_agentClassesToAgentImplementationSubscriptions;
  static inline std::unordered_map<std::string, std::pair<ScAddr, ScAddr>> m_agentEventClasses;

  template <typename T>
  using Ref = std::reference_wrapper<T>;
  using ScAgentImplementationsToSubscriptionsRef = Ref<ScAgentManager<TScAgent>::ScAgentImplementationsToSubscriptions>;
  using ScSubscriptionsRef = Ref<ScSubscriptions>;
  template <typename T>
  using OptionalRef = std::optional<std::reference_wrapper<T>>;
  using ScAgentImplementationsToSubscriptionsOptionalRef =
      OptionalRef<ScAgentManager<TScAgent>::ScAgentImplementationsToSubscriptions>;
  using ScSubscriptionsOptionalRef = OptionalRef<ScSubscriptions>;

  /*!
   * @brief Retrieves the class name of a specified agent.
   *
   * If specified agent has abstract agent class then this method returns its system identifier. Otherwise, it returns
   * agent class name.
   *
   * @param context A pointer to the ScMemoryContext used for accessing
   *                system identifiers.
   * @param agent A reference to the agent whose class name is to be retrieved.
   * @return A string representing the class name of the agent.
   */
  static std::string GetAgentClassName(ScMemoryContext * context, TScAgent & agent);

  /*!
   * @brief Retrieves the name of the agent implementation.
   *
   * If the identifier of agent implementation is not found, it returns its hash as a string.
   *
   * @param context A pointer to the ScMemoryContext used for accessing
   *                system identifiers.
   * @param agentImplementationAddr An address of the agent implementation in the memory context.
   * @return A string representing the name of the agent implementation. If no name is found,
   *         it returns the hash of the address as a string.
   */
  static std::string GetAgentImplementationName(ScMemoryContext * context, ScAddr const & agentImplementationAddr);

  /*!
   * @brief Retrieves detailed information about an agent's implementation.
   *
   * This function constructs a string that describes the agent implementation, including its relationship to an
   * abstract agent class. If the provided agent implementation is valid, it includes both the implementation name and
   * the abstract agent class name; otherwise, it only includes the class name.
   *
   * @param context A pointer to the ScMemoryContext used for checking element validity and retrieving names.
   * @param agent A reference to an instance of TScAgent whose class name is used in the output.
   * @param agentImplementationAddr An address of the agent implementation in memory context.
   * @return A string containing information about the agent implementation and its associated abstract agent.
   */
  static std::string GetAgentImplementationInfo(
      ScMemoryContext * context,
      TScAgent & agent,
      ScAddr const & agentImplementationAddr);

  /*!
   * @brief Retrieves the name of a subscription element.
   *
   * This method obtains the system identifier for a given subscription element. If the identifier is not found, it
   * falls back to returning a hash of its address.
   *
   * @param context A pointer to the ScMemoryContext used for accessing
   *                system identifiers.
   * @param subscriptionElementAddr An address of the subscription element
   *                                whose name is to be retrieved.
   * @return A string representing the name of the subscription element.
   */
  static std::string GetSubscriptionElementName(ScMemoryContext * context, ScAddr const & eventSubscriptionAddr);

  /*!
   * @brief Retrieves the event class name associated with an agent.
   *
   * This method checks if the template type TScEvent is an instance of ScElementaryEvent. If so, it retrieves the event
   * class from the agent and returns system identifier of this event class. Otherwise, it returns event class name.
   *
   * @param context A pointer to the ScMemoryContext used for accessing
   *                system identifiers.
   * @param agent A reference to the agent whose event class name is to be retrieved.
   * @return A string representing the event class name associated with the agent.
   */
  static std::string GetEventClassName(ScMemoryContext * context, TScAgent & agent);

  /*!
   * @brief Checks if an agent of a specified class is subscribed to the event of an element being erased.
   *
   * This method looks up the provided agent class name in the internal map of agent event classes.
   * It determines if the agent class is associated with the specific event that indicates an element
   * is about to be erased and if subscription sc-element of this event is erased.
   *
   * @param context A sc-memory context used to check for subscription to the erase event.
   * @param agentClassName The name of the agent class to check for subscription to the erase event.
   *
   * @return true if the specified agent class was subscribed to the event of an element being erased and subscription
   * sc-element of this event is erased; false otherwise.
   */
  static _SC_EXTERN bool WasAgentSubscribedToEventOfErasedElementErasing(
      ScMemoryContext * context,
      std::string const & agentClassName);

  /*!
   * @brief Resolves the subscriptions for agent implementations associated with a given agent class name.
   *
   * This function checks if there are existing subscriptions for the specified agent class name.
   * If not, it initializes an empty subscription list for that class name. It returns a reference
   * to the subscriptions associated with the agent class.
   *
   * @param agentClassName A name of the agent class for which to resolve subscriptions.
   * @return A reference to the subscriptions associated with the specified agent class.
   */
  static _SC_EXTERN ScAgentImplementationsToSubscriptionsRef
  ResolveAgentClassAgentImplementationSubscriptions(std::string const & agentClassName);

  /*!
   * @brief Retrieves the subscriptions for agent implementations associated with a given agent class name.
   *
   * This function looks up the subscriptions for the specified agent class name. If found, it returns
   * a reference to the subscriptions; otherwise, it returns an empty optional.
   *
   * @param agentClassName A name of the agent class for which to retrieve subscriptions.
   * @return A reference to the subscriptions associated with the specified agent class, or an empty optional if not
   * found.
   */
  static _SC_EXTERN ScAgentImplementationsToSubscriptionsOptionalRef
  GetAgentClassAgentImplementationSubscriptions(std::string const & agentClassName);

  /*!
   * @brief Generates subscriptions for a specific agent implementation.
   *
   * This function creates a new entry for the specified agent implementation address in the subscriptions map.
   * It initializes an empty subscription list for that implementation and returns a reference to the subscriptions.
   *
   * @param agentImplementationAddr A sc-address of the agent implementation for which to generate subscriptions.
   * @param agentImplementationsToSubscriptions A reference to the map of agent implementations to their subscriptions.
   * @return A reference to the subscriptions associated with the specified agent implementation.
   */
  static _SC_EXTERN ScSubscriptionsRef GenerateAgentImplementationSubscriptions(
      ScAddr const & agentImplementationAddr,
      ScAgentImplementationsToSubscriptions & agentImplementationsToSubscriptions);

  /*!
   * @brief Retrieves the subscriptions associated with a specific agent implementation.
   *
   * This function looks up the subscriptions for the specified agent implementation address. If found,
   * it returns a reference to the subscriptions; otherwise, it returns an empty optional.
   *
   * @param agentImplementationAddr A sc-address of the agent implementation for which to retrieve subscriptions.
   * @param agentImplementationsToSubscriptions A reference to the map of agent implementations to their subscriptions.
   * @return A reference to the subscriptions associated with the specified agent implementation, or an empty optional
   * if not found.
   */
  static _SC_EXTERN ScSubscriptionsOptionalRef GetAgentImplementationSubscriptions(
      ScAddr const & agentImplementationAddr,
      ScAgentImplementationsToSubscriptions & agentImplementationsToSubscriptions);

  /*!
   * @brief Retrieves a specific subscription based on its subscription sc-element.
   *
   * This function searches for a subscription associated with the provided event subscription sc-element sc-address
   * within the specified subscriptions. If found, it returns a pointer to the subscription; otherwise, it returns
   * nullptr.
   *
   * @param eventSubscriptionElementAddr A sc-address of the event subscription element to search for.
   * @param subscriptions A reference to the map of subscriptions to search within.
   * @return A pointer to the subscription if found, or nullptr if not found.
   */
  static _SC_EXTERN ScEventSubscription * GetSubscription(
      ScAddr const & eventSubscriptionElementAddr,
      ScSubscriptions const & subscriptions);

  /*!
   * @brief Erases a specific subscription from the subscriptions map.
   *
   * This function removes the subscription associated with the provided event subscription element address
   * from the specified subscriptions.
   *
   * @param eventSubscriptionElementAddr A sc-address of the event subscription element to be erased.
   * @param subscriptions A reference to the map of subscriptions from which to erase the subscription.
   */
  static _SC_EXTERN void EraseSubscription(
      ScAddr const & eventSubscriptionElementAddr,
      ScSubscriptions & subscriptions);

  /*!
   * @brief Clears empty subscriptions for a specific agent implementation and class.
   *
   * This function checks if the subscriptions for the specified agent implementation are empty. If they are,
   * it removes the implementation from the agent implementations map. If the entire agent class has no implementations
   * left, it removes the class from the agent classes map.
   *
   * @param agentClassName A name of the agent class to check for empty implementations.
   * @param agentImplementationAddr A sc-address of the agent implementation to check for empty subscriptions.
   * @param agentImplementationsToSubscriptions A reference to the map of agent implementations to their subscriptions.
   * @param subscriptions A reference to the subscriptions associated with the agent implementation.
   */
  static _SC_EXTERN void ClearEmptyAgentImplementationSubscriptions(
      std::string const & agentClassName,
      ScAddr const & agentImplementationAddr,
      ScAgentManager<TScAgent>::ScAgentImplementationsToSubscriptionsOptionalRef & agentImplementationsToSubscriptions,
      ScSubscriptionsOptionalRef const & subscriptions);

  //! Returns callback that remove subscription of agent to sc-event of erasing sc-element from common map.
  static _SC_EXTERN std::function<void(void)> GetPostEraseEventCallback(
      std::string const & agentName,
      std::string const & eventClassName,
      ScAddr const & agentImplementationAddr,
      ScAddr const & subscriptionElementAddr);

  /*!
   * @brief Gets the callback function for agent class.
   * @tparam TScAgent An agent class to be subscribed to the event.
   * @param agentImplementationAddr A sc-address of agent implementation specified in knowledge base for this agent.
   * @param postEraseEventCallback A callback function that remove subscription of agent to sc-event of erasing
   * sc-element from common map after agent flow of performing action. class.
   * @return A function that takes an sc-event and returns an sc-result.
   * @warning Specified agent class must be derived from class `ScAgent`.
   */
  static _SC_EXTERN std::function<void(TScEvent const &)> GetCallback(
      ScAddr const & agentImplementationAddr,
      std::function<void(void)> const & postEraseEventCallback) noexcept;
};

#include "_template/sc_agent_manager.tpp"
