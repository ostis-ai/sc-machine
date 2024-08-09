/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "sc_addr.hpp"

#include "sc_memory.hpp"

class ScKeynodes;
class ScActionAgent;
class ScAgentBuilderAbstract;
template <class TScAgent>
class ScAgentBuilder;

/*!
 * @class ScModule
 * @brief This class is an interface for keynodes and agent modules. It's like a complex component that contains linked
 * agents.
 *
 * Derive this class to implement your own module class.
 *
 * @code
 * // File my_module.hpp
 * #pragma once
 *
 * #include <sc-memory/sc_module.hpp>
 *
 * class MyModule final : public ScModule
 * {
 * };
 *
 * // File my_module.cpp:
 * #include "my-module/my_module.hpp"
 *
 * #include "my-module/keynodes/my_keynodes.hpp"
 * #include "my-module/agents/my_agent.hpp"
 *
 * SC_MODULE_REGISTER(MyModule)
 *   ->Keynodes<MyKeynodes>()
 *   ->Agent<MyAgent>();
 *
 * \endcode
 * @note Not recommended to use interface API to implement module classes. Use example defines instead.
 */
class _SC_EXTERN ScModule : public ScObject
{
public:
  _SC_EXTERN ~ScModule() override;

  _SC_EXTERN static ScModule * Create(ScModule * module);

  /*!
   * @brief Reminds keynodes to register it with module after.
   * @param TKeynodesClass A keynodes class to be initialized in this module.
   * @returns A pointer to module instance.
   */
  template <class TKeynodesClass>
  _SC_EXTERN ScModule * Keynodes();

  /*!
   * @brief Reminds agent and it initiation condition to register it with module after.
   * @param TScAgent An agent class to be subscribe to.
   * @param subscriptionAddrs A list of sc-addresses of sc-elements to subscribe to.
   * @returns A pointer to module instance.
   */
  template <
      class TScAgent,
      class... TScAddr,
      typename = std::enable_if<!std::is_base_of<ScActionAgent, TScAgent>::value>>
  _SC_EXTERN ScModule * Agent(TScAddr const &... subscriptionAddrs);

  /*!
   * @brief Reminds action agent and it initiation condition to register it with module after.
   * @param TScAgent An agent class to be subscribe to.
   * @param subscriptionAddrs A list of sc-addresses of sc-elements to subscribe to.
   * @returns A pointer to module instance.
   */
  template <class TScAgent, typename = std::enable_if<std::is_base_of<ScActionAgent, TScAgent>::value>>
  _SC_EXTERN ScModule * Agent();

  /*!
   * @brief Creates an agent builder for the specified agent implementation.
   * @param TScAgent An agent class to be subscribe to.
   * @param agentImplementationAddr A sc-address of the agent implementation.
   * @return A pointer to the created agent builder.
   */
  template <class TScAgent>
  _SC_EXTERN ScAgentBuilder<TScAgent> * AgentBuilder(ScAddr const & agentImplementationAddr = ScAddr::Empty);

  /*!
   * @brief Registers all module keynodes and agents.
   * @returns Result of initializing.
   */
  _SC_EXTERN void Register(ScMemoryContext * ctx);

  /*! Unregisters all module keynodes and agents.
   * @returns Result of shutdown.
   */
  _SC_EXTERN void Unregister(ScMemoryContext * ctx);

  /*!
   * @brief Initializes the module with the given memory context.
   * @param ctx A sc-memory context for initialization.
   */
  _SC_EXTERN virtual void Initialize(ScMemoryContext * ctx);

  /*!
   * @brief Shuts down the module with the given memory context.
   * @param A The sc-memory context for shutdown.
   */
  _SC_EXTERN virtual void Shutdown(ScMemoryContext * ctx);

protected:
  /// Registered keynodes
  std::list<ScKeynodes *> m_keynodes;
  /// Registered agents
  using ScAgentSubscribeCallback = std::function<void(ScMemoryContext *, ScAddr const &, ScAddrVector const &)>;
  using ScAgentUnsubscribeCallback = std::function<void(ScMemoryContext *, ScAddr const &, ScAddrVector const &)>;
  std::list<std::tuple<ScAgentBuilderAbstract *, ScAgentSubscribeCallback, ScAgentUnsubscribeCallback, ScAddrVector>>
      m_agents;

  template <class TScAgent>
  ScAgentSubscribeCallback GetAgentSubscribeCallback();

  template <class TScAgent>
  ScAgentUnsubscribeCallback GetAgentUnsubscribeCallback();
};

/// Registers module class instance
#define SC_MODULE_REGISTER(__ModuleName__) \
  struct __ModuleName__##Dummy \
  { \
    static ScModule * ms_module; \
  }; \
  extern "C" \
  { \
  _SC_EXTERN sc_result sc_module_initialize() \
  { \
    try \
    { \
      __ModuleName__##Dummy::ms_module->Register(ScMemory::ms_globalContext); \
    } \
    catch (utils::ScException const & e) \
    { \
      SC_LOG_ERROR(__ModuleName__##Dummy::ms_module->GetName() << ": " << e.Message()); \
      return SC_RESULT_ERROR; \
    } \
    return SC_RESULT_OK; \
  } \
\
  _SC_EXTERN sc_result sc_module_shutdown() \
  { \
    try \
    { \
      __ModuleName__##Dummy::ms_module->Unregister(ScMemory::ms_globalContext); \
    } \
    catch (utils::ScException const & e) \
    { \
      SC_LOG_ERROR(__ModuleName__##Dummy::ms_module->GetName() << ": " << e.Message()); \
      return SC_RESULT_ERROR; \
    } \
    return SC_RESULT_OK; \
  } \
  } \
  ScModule * __ModuleName__##Dummy::ms_module = __ModuleName__::Create(new __ModuleName__())

#include "sc_module.tpp"
