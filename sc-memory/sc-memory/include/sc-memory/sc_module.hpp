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
class ScActionInitiatedAgent;
class ScAgentBuilderAbstract;
template <class TScAgent>
class ScAgentBuilder;

/*!
 * @class ScModule
 * @brief A base class for registering agents. It's like a complex component that contains
 * connected agents.
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
 * #include "my-module/agent/my_agent.hpp"
 *
 * SC_MODULE_REGISTER(MyModule)
 *   ->Agent<MyAgent>();
 *
 * \endcode
 * @note Not recommended to use interface API to implement module classes. Use `SC_MODULE_REGISTER` instead.
 */
class _SC_EXTERN ScModule : public ScObject
{
public:
  _SC_EXTERN ~ScModule() noexcept override;

  _SC_EXTERN static ScModule * Create(ScModule * module) noexcept;

  /*!
   * @brief Remembers agent and it initiation condition to register it with module after.
   * @tparam TScAgent An agent class to subscribe.
   * @param subscriptionAddrs A list of sc-addresses of sc-elements to subscribe to.
   * @returns A pointer to module instance.
   */
  template <
      class TScAgent,
      class... TScAddr,
      typename = std::enable_if<!std::is_base_of<ScActionInitiatedAgent, TScAgent>::value>>
  _SC_EXTERN ScModule * Agent(TScAddr const &... subscriptionAddrs) noexcept;

  /*!
   * @brief Remembers action agent and it initiation condition to register it with module after.
   * @tparam TScAgent An agent class to subscribe.
   * @returns A pointer to module instance.
   */
  template <class TScAgent, typename = std::enable_if<std::is_base_of<ScActionInitiatedAgent, TScAgent>::value>>
  _SC_EXTERN ScModule * Agent() noexcept;

  /*!
   * @brief Generates an agent builder for the specified agent implementation.
   * @tparam TScAgent An agent class to subscribe.
   * @param agentImplementationAddr A sc-address of the agent implementation.
   * @return A pointer to the created agent builder.
   */
  template <class TScAgent>
  _SC_EXTERN ScAgentBuilder<TScAgent> * AgentBuilder(ScAddr const & agentImplementationAddr = ScAddr::Empty) noexcept;

  /*!
   * @brief Subscribes all module agents.
   * @param context A sc-memory context for registering.
   * @returns Result of initializing.
   */
  _SC_EXTERN void Register(ScMemoryContext * context) noexcept(false);

  /*! Unsubscribes all module agents.
   * @param context A sc-memory context for unregistering.
   * @returns Result of shutdown.
   */
  _SC_EXTERN void Unregister(ScMemoryContext * context) noexcept(false);

  /*!
   * @brief Initializes the module with the given memory context.
   * @param context A sc-memory context for initialization.
   */
  _SC_EXTERN virtual void Initialize(ScMemoryContext * context);

  /*!
   * @brief Shuts down the module with the given memory context.
   * @param context A sc-memory context for shutdown.
   */
  _SC_EXTERN virtual void Shutdown(ScMemoryContext * context);

protected:
  /// Registered agents
  using ScAgentSubscribeCallback = std::function<void(ScMemoryContext *, ScAddr const &, ScAddrVector const &)>;
  using ScAgentUnsubscribeCallback = std::function<void(ScMemoryContext *, ScAddr const &, ScAddrVector const &)>;
  std::list<std::tuple<ScAgentBuilderAbstract *, ScAgentSubscribeCallback, ScAgentUnsubscribeCallback, ScAddrVector>>
      m_agents;

  template <class TScAgent>
  ScAgentSubscribeCallback GetAgentSubscribeCallback() noexcept;

  template <class TScAgent>
  ScAgentUnsubscribeCallback GetAgentUnsubscribeCallback() noexcept;
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

#include "_template/sc_module.tpp"
