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
   * @param ctx A sc-memory context for initialization.
   */
  _SC_EXTERN virtual void Initialize(ScMemoryContext * ctx) = 0;

  /*!
   * @brief Shuts down the agent builder with the given memory context.
   * @param ctx A sc-memory context for shutdown.
   */
  _SC_EXTERN virtual void Shutdown(ScMemoryContext * ctx) = 0;
};

/*!
 * @class ScAgentBuilder
 * @brief A class for building agents.
 *
 * This class extends ScAgentBuilderAbstract and provides methods for setting various properties of the agent and
 * finalizing the build process.
 *
 * ScAgentBuilder can be used to load existing specification of specified sc-agent of generate new one.
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
  _SC_EXTERN ScAgentBuilder(ScAddr const & agentImplementationAddr);

  /*!
   * @brief Constructs an ScAgentBuilder with the given module and agent implementation.
   * @param module A module associated with the agent.
   * @param agentImplementationAddr A sc-address of the agent implementation.
   */
  _SC_EXTERN ScAgentBuilder(ScModule * module, ScAddr const & agentImplementationAddr);

  /*!
   * @brief Sets abstract agent address for specified agent class `TScAgent`.
   * @param abstractAgentAddr A sc-address of abstract agent.
   * @return The current instance of ScAgentBuilder.
   * @throws utils::ExceptionInvalidState if the specified abstract agent is not valid.
   */
  _SC_EXTERN ScAgentBuilder * SetAbstractAgent(ScAddr const & abstractAgentAddr);

  /*!
   * @brief Sets primary initiation condition for specified agent class `TScAgent`.
   * @param primaryInitiationCondition A tuple containing sc-event class and sc-event subscription sc-element.
   * @return The current instance of ScAgentBuilder.
   * @throws utils::ExceptionInvalidState if the specified event class or subscription element is not valid.
   */
  _SC_EXTERN ScAgentBuilder * SetPrimaryInitiationCondition(
      std::tuple<ScAddr, ScAddr> const & primaryInitiationCondition);

  /*!
   * @brief Sets action class for specified agent class `TScAgent`.
   * @param actionClassAddr A sc-address of the action class.
   * @return The current instance of ScAgentBuilder.
   * @throws utils::ExceptionInvalidState if the specified action class is not valid.
   */
  _SC_EXTERN ScAgentBuilder * SetActionClass(ScAddr const & actionClassAddr);

  /*!
   * @brief Sets initiation condition and result for specified agent class `TScAgent`
   * @param initiationConditionAndResult A tuple containing initiation condition and result condition
   * @return The current instance of ScAgentBuilder.
   * @throws utils::ExceptionInvalidState if the specified initiation or result condition is not valid.
   */
  _SC_EXTERN ScAgentBuilder * SetInitiationConditionAndResult(
      std::tuple<ScAddr, ScAddr> const & initiationConditionAndResult);

  /*!
   * @brief Finalizes build process of specification for specified agent class `TScAgent` and returns the associated
   * module.
   * @return A module associated with the built agent.
   */
  _SC_EXTERN ScModule * FinishBuild();

protected:
  using ScInitializeCallback = std::function<void(ScMemoryContext *)>;

  ScModule * m_module;

  ScAddr m_agentImplementationAddr;

  // abstract agent
  ScAddr m_abstractAgentAddr;
  ScInitializeCallback m_initializeAbstractAgent;

  // primary initiation condition
  ScAddr m_eventClassAddr;
  ScAddr m_eventSubscriptionElementAddr;
  ScInitializeCallback m_initializePrimaryInitiationCondition;

  // action class
  ScAddr m_actionClassAddr;
  ScInitializeCallback m_initializeActionClass;

  // initiation condition and result
  ScAddr m_initiationConditionAddr;
  ScAddr m_resultConditionAddr;
  ScInitializeCallback m_initializeInitiationConditionAndResult;

  ScInitializeCallback m_initializeSpecification;

  /*!
   * @brief Gets agent implementation for specified agent class `TScAgent`.
   * @return A sc-address of agent implementation.
   */
  _SC_EXTERN ScAddr GetAgentImplementation() const override;

  /*!
   * @brief Loads specification for specified agent class `TScAgent`.
   * @param ctx A sc-memory context used to load specification.
   */
  _SC_EXTERN void LoadSpecification(ScMemoryContext * ctx);

  _SC_EXTERN void ResolveAgentImplementation(
      ScMemoryContext * ctx,
      std::string & agentImplementationName,
      std::string const & agentClassName);

  _SC_EXTERN void ResolveAbstractAgent(
      ScMemoryContext * ctx,
      std::string const & agentImplementationName,
      std::string const & agentClassName);

  _SC_EXTERN void ResolvePrimaryInitiationCondition(
      ScMemoryContext * ctx,
      std::string const & abstractAgentName,
      std::string const & agentClassName);

  _SC_EXTERN void ResolveActionClass(
      ScMemoryContext * ctx,
      std::string const & abstractAgentName,
      std::string const & agentClassName);

  _SC_EXTERN void ResolveInitiationConditionAndResultCondition(
      ScMemoryContext * ctx,
      std::string const & abstractAgentName,
      std::string const & agentClassName);

  /*!
   * @brief Initializes the agent builder with the given memory context.
   * @param ctx A sc-memory context for initialization.
   */
  _SC_EXTERN void Initialize(ScMemoryContext * ctx) override;

  /*!
   * @brief Shuts down the agent builder with the given memory context.
   * @param ctx A sc-memory context for shutdown.
   */
  _SC_EXTERN void Shutdown(ScMemoryContext * ctx) override;
};

#include "sc_agent_builder.tpp"
