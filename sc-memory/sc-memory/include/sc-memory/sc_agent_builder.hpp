/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <functional>

#include "sc_object.hpp"

#include "sc_addr.hpp"

class ScModule;
class ScMemoryContext;
class ScAgentContext;

/*!
 * @class ScAgentBuilderAbstract
 * @brief An abstract base class for sc-agent builders.
 *
 * This class provides a base interface for sc-agent builders, offering methods for initialization and shutdown.
 */
class ScAgentBuilderAbstract : public ScObject
{
public:
  /*!
   * @brief Gets agent implementation for specified agent class.
   * @return A sc-address of agent implementation.
   */
  virtual ScAddr GetAgentImplementation() const = 0;

  /*!
   * @brief Initializes the agent builder with the given memory context.
   * @param context A sc-memory context for initialization.
   */
  _SC_EXTERN virtual void Initialize(ScMemoryContext * context) = 0;

  /*!
   * @brief Shuts down the agent builder with the given memory context.
   * @param context A sc-memory context for shutdown.
   */
  _SC_EXTERN virtual void Shutdown(ScMemoryContext * context) = 0;
};

/*!
 * @class ScAgentBuilder
 * @brief A class for building agents.
 *
 * This class extends ScAgentBuilderAbstract and provides methods for setting various properties of the agent and
 * finalizing the build process.
 *
 * ScAgentBuilder can be used to load existing specification of specified sc-agent or generate new one.
 *
 * @tparam TScAgent A type of sc-agent to be built.
 */
template <class TScAgent>
class ScAgentBuilder : public ScAgentBuilderAbstract
{
  friend class ScModule;
  friend class ScAgentContext;

public:
  /*!
   * @brief Constructs an ScAgentBuilder with the given agent implementation.
   * @param agentImplementationAddr A sc-address of the agent implementation.
   */
  _SC_EXTERN ScAgentBuilder(ScAddr const & agentImplementationAddr) noexcept;

  /*!
   * @brief Constructs an ScAgentBuilder with the given module and agent implementation.
   * @param module A module associated with the agent.
   * @param agentImplementationAddr A sc-address of the agent implementation.
   */
  _SC_EXTERN ScAgentBuilder(ScModule * module, ScAddr const & agentImplementationAddr) noexcept;

  /*!
   * @brief Sets abstract agent address for specified agent class `TScAgent`.
   * @param abstractAgentAddr A sc-address of abstract agent.
   * @return The current instance of ScAgentBuilder.
   * @throws utils::ExceptionInvalidParams if the specified abstract agent is not valid.
   */
  _SC_EXTERN ScAgentBuilder * SetAbstractAgent(ScAddr const & abstractAgentAddr) noexcept;

  /*!
   * @brief Sets primary initiation condition for specified agent class `TScAgent`.
   * @param primaryInitiationCondition A tuple containing sc-event class and sc-event subscription sc-element.
   * @return The current instance of ScAgentBuilder.
   * @throws utils::ExceptionInvalidParams if the specified event class or subscription element is not valid.
   */
  _SC_EXTERN ScAgentBuilder * SetPrimaryInitiationCondition(
      std::tuple<ScAddr, ScAddr> const & primaryInitiationCondition) noexcept;

  /*!
   * @brief Sets action class for specified agent class `TScAgent`.
   * @param actionClassAddr A sc-address of the action class.
   * @return The current instance of ScAgentBuilder.
   * @throws utils::ExceptionInvalidParams if the specified action class is not valid.
   */
  _SC_EXTERN ScAgentBuilder * SetActionClass(ScAddr const & actionClassAddr) noexcept;

  /*!
   * @brief Sets initiation condition and result for specified agent class `TScAgent`
   * @param initiationConditionAndResult A tuple containing initiation condition and result condition
   * @return The current instance of ScAgentBuilder.
   * @throws utils::ExceptionInvalidParams if the specified initiation or result condition is not valid.
   */
  _SC_EXTERN ScAgentBuilder * SetInitiationConditionAndResult(
      std::tuple<ScAddr, ScAddr> const & initiationConditionAndResult) noexcept;

  /*!
   * @brief Finalizes build process of specification for specified agent class `TScAgent` and returns the associated
   * module.
   * @return A module associated with the built agent.
   */
  _SC_EXTERN ScModule * FinishBuild() noexcept;

protected:
  using ScInitializeCallback = std::function<void(ScMemoryContext *)>;

  ScModule * m_module;

  ScAddr m_agentImplementationAddr;

  // abstract agent
  ScAddr m_abstractAgentAddr;
  ScInitializeCallback m_checkAbstractAgent;

  // primary initiation condition
  ScAddr m_eventClassAddr;
  ScAddr m_eventSubscriptionElementAddr;
  ScInitializeCallback m_checkPrimaryInitiationCondition;

  // action class
  ScAddr m_actionClassAddr;
  ScInitializeCallback m_checkActionClass;

  // initiation condition and result
  ScAddr m_initiationConditionAddr;
  ScAddr m_resultConditionAddr;
  ScInitializeCallback m_checkInitiationConditionAndResult;

  ScInitializeCallback m_resolveSpecification;

  /*!
   * @brief Gets agent implementation for specified agent class `TScAgent`.
   * @return A sc-address of agent implementation.
   */
  _SC_EXTERN ScAddr GetAgentImplementation() const noexcept override;

  /*!
   * @brief Resolves specification for specified agent class `TScAgent`.
   *
   * This method calls `ResolveAgentImplementation`, `ResolveAbstractAgent`, `ResolvePrimaryInitiationCondition`,
   * `ResolveActionClass`, `ResolveInitiationConditionAndResultCondition`.
   *
   * @param context A sc-memory context used to load specification.
   */
  _SC_EXTERN void ResolveSpecification(ScMemoryContext * context) noexcept(false);

  /*!
   * @brief Resolves the agent implementation for a given agent class.
   *
   * If `agent_implementation` is provided by `AgentBuilder` method, then this method retrieves system identifier for
   * `agent_implementation` and validates that `agent_implementation` belongs to `platform_dependent_abstract_sc_agent`.
   * If the implementation was not provided by `AgentBuilder` method, this method generates a new node for
   * implementation and establishes belonging to `platform_dependent_abstract_sc_agent.
   *
   * @code
   *    platform_dependent_abstract_sc_agent
   *                  |
   *                  |
   *                  |
   *                 \/
   *        agent_implementation
   * @endcode
   *
   * @param context A pointer to the memory context used for operations.
   * @param agentImplementationName A name of agent implementation.
   * @param agentClassName A name of the agent class.
   *
   * @throws utils::ExceptionInvalidState If specified `agent_implementation` does not belong to
   * `platform_dependent_abstract_sc_agent` or if it belongs to it twice.
   */
  _SC_EXTERN void ResolveAgentImplementation(
      ScMemoryContext * context,
      std::string & agentImplementationName,
      std::string const & agentClassName) noexcept(false);

  /*!
   * @brief Resolves an abstract agent associated with a given agent implementation.
   *
   * If `abstract_agent` is provided by `SetAbstractAgent` method, then this method verifies a connection between
   * `abstract_agent` and `agent_implementation`. It ensures that the connection does not already exist and generates it
   * if necessary. If `abstract_agent` was not provided by `SetAbstractAgent` method, then this method searches this one
   * for specified `agent_implementation`. If the `agent_implementation` does not have abstract agent, then an exception
   * is thrown.
   *
   * @code
   *             nrel_inclusion
   *                   |
   *                   |
   *                   |
   *                  \/
   * abstract_agent ======> agent_implementation
   * @endcode
   *
   * @param context A pointer to memory context used for operations.
   * @param agentImplementationName A name of agent implementation.
   * @param agentClassName A name of agent class.
   *
   * @throws utils::ExceptionInvalidState If a connection between `abstract_agent` and `agent_implementation` is
   * invalid or does not exist.
   */
  _SC_EXTERN void ResolveAbstractAgent(
      ScMemoryContext * context,
      std::string const & agentImplementationName,
      std::string const & agentClassName) noexcept(false);

  /*!
   * @brief Resolves primary initiation condition for a given abstract agent.
   *
   * If primary initiation condition is provided by `SetPrimaryInitiationCondition` method then this method checks if
   * primary initiation condition already exists for `abstract_agent`. If it does not exist, it generates necessary
   * connectors to establish condition. If primary initiation condition was npt provided by
   * `SetPrimaryInitiationCondition` method then this method searches initiation condition for `abstract_agent`. If the
   * `abstract_agent` does not have primary initiation condition, then an exception is thrown.
   *
   * @code
   *     nrel_primary_initiation_condition
   *                    |
   *                    |
   *                    | event_class
   *                    \/    ||
   *  abstract_agent =======> ||
   *                          ||
   *                          \/
   *               event_subscription_element
   * @endcode
   *
   * @param context A pointer to memory context used for operations.
   * @param abstractAgentName A name of `abstract_agent`.
   * @param agentClassName A name of agent class.
   *
   * @throws utils::ExceptionInvalidState If `abstract_agent` does not have a valid primary initiation condition.
   */
  _SC_EXTERN void ResolvePrimaryInitiationCondition(
      ScMemoryContext * context,
      std::string const & abstractAgentName,
      std::string const & agentClassName) noexcept(false);

  /*!
   * @brief Resolves action class associated with a given abstract agent.
   *
   * If `action_class` is provided by `SetActionClass` method then this method checks if `action_class` is already
   * associated with `abstract_agent`. If not, it generates necessary connectors to establish the connection. If
   * `action_class` was not provided by `SetActionClass` method then this method searches this one for `abstract_agent`.
   * If the `abstract_agent` does not have action class, then an exception is thrown.
   *
   * @code
   *        nrel_sc_agent_action_class
   *                   |
   *                   |
   *                   |
   *                   \/
   *  abstract_agent ======> action_class
   * @endcode
   *
   * @param context A pointer to memory context used for operations.
   * @param abstractAgentName A name of abstract agent.
   * @param agentClassName A name of the agent class.
   *
   * @throws utils::ExceptionInvalidState If `abstract_agent` does not have a valid action class.
   */
  _SC_EXTERN void ResolveActionClass(
      ScMemoryContext * context,
      std::string const & abstractAgentName,
      std::string const & agentClassName) noexcept(false);

  /*!
   * @brief Resolves the initiation condition and result condition for a given abstract agent.
   *
   * If initiation and result conditions are provided by `SetInitiationConditionAndResult`, then this method
   * checks if both initiation condition and result condition already exist for `abstract_agent`. If they do not exist,
   * it generates connectors to establish the conditions. If initiation and result conditions were not provided
   * by `SetInitiationConditionAndResult`, then this method searches both conditions for `abstract_agent`. If the
   * `abstract_agent` does not have initiation condition and result condition, then an exception is thrown.
   *
   *    nrel_initiation_condition_and_result
   *                   |
   *                   |
   *                   | initiation_condition
   *                   \/       ||
   *  abstract_agent =========> ||
   *                            ||
   *                            \/
   *                       result_condition
   *
   * @param context A pointer to the memory context used for operations.
   * @param abstractAgentName A constant reference to the name of the abstract agent.
   * @param agentClassName A constant reference to the name of the agent class.
   *
   * @throws utils::ExceptionInvalidState If `abstract_agent` does not have valid initiation and result conditions.
   */
  _SC_EXTERN void ResolveInitiationConditionAndResultCondition(
      ScMemoryContext * context,
      std::string const & abstractAgentName,
      std::string const & agentClassName) noexcept(false);

  /*!
   * @brief Initializes the agent builder with the given memory context.
   * @param context A sc-memory context for initialization.
   */
  _SC_EXTERN void Initialize(ScMemoryContext * context) noexcept(false) override;

  /*!
   * @brief Shuts down the agent builder with the given memory context.
   * @param context A sc-memory context for shutdown.
   */
  _SC_EXTERN void Shutdown(ScMemoryContext * context) noexcept(false) override;
};

#include "_template/sc_agent_builder.tpp"
