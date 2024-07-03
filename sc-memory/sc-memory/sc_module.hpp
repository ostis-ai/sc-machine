/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_object.hpp"
#include "sc-memory/sc_addr.hpp"
#include "sc_agent.hpp"

/*!
 * @interface An interface for keynodes and agent modules. It's like a complex component that contains linked agents.
 * Implement your own module class.
 * @example
 * File sc_nlp_module.hpp:
 * \code
 * #include "sc-memory/kpm/sc_module.hpp"
 *
 * #include "nlp-module/keynodes/sc_nlp_keynodes.hpp"
 * #include "nlp-module/agents/sc_syntactic_analysis_agent.hpp"
 * #include "nlp-module/agents/sc_semantic_analysis_agent.hpp"
 *
 * SC_MODULE(ScNLPModule)
 *   ->Keynodes(new nlp::ScNLPKeynodes())
 *   ->Agent(
 *      new nlp::ScSyntacticAnalysisAgent<ScEvent::Type::AddOutputEdge>(),
 *      nlp::ScNLPKeynodes::kSyntacticAnalysisAction,
 *   )
 *   ->Agent(
 *      new nlp::ScSemanticAnalysisAgent<ScEvent::Type::AddOutputEdge>(),
 *      nlp::ScNLPKeynodes::kSemanticAnalysisAction
 *   );
 * \endcode
 * File sc_nlp_module.cpp:
 * \code
 * #include "nlp-module/sc_nlp_module.hpp"
 *
 * SC_MODULE_REGISTER(SC_NLP_MODULE);
 * \endcode
 * @note Not recommended to use interface API to implement module classes. Use example defines instead of.
 */
class _SC_EXTERN ScModule : public ScObject
{
public:
  _SC_EXTERN ~ScModule() override = default;

  static _SC_EXTERN std::string GetName()
  {
    return "ScModule";
  }

  _SC_EXTERN static ScModule * Create(ScModule * module)
  {
    return module;
  }

  /*! Reminds keynodes to register it with module after.
   * @param keynodes A pointer to dynamically created keynodes instance
   * @returns Pointer to module instance
   */
  template <class KeynodesClass>
  _SC_EXTERN ScModule * Keynodes()
  {
    static_assert(
        std::is_base_of<ScKeynodes, KeynodesClass>::value, "KeynodesClass must be derivied from ScKeynodes class");
    m_keynodes.push_back(new KeynodesClass());
    return this;
  }

  /*! Reminds agent and it initiation condition to register it with module after.
   * @param agentInfo A pointer to dynamically created agent instance and a vector of subscription addrs
   * @returns Pointer to module instance
   */
  template <class AgentClass>
  _SC_EXTERN ScModule * Agent(ScAddrVector const & addrs)
  {
    static_assert(
        std::is_base_of<ScAgentAbstract, AgentClass>::value, "AgentClass must be derivied from ScAgentAbstract class");
    m_agents.push_back({{GetAgentRegisterCallback<AgentClass>(), GetAgentUnregisterCallback<AgentClass>()}, addrs});
    return this;
  }

  /*! Reminds agent and it initiation condition to register it with module after.
   * @param agentInfo A pointer to dynamically created agent instance and a subscription addr
   * @returns Pointer to module instance
   */
  template <class AgentClass>
  _SC_EXTERN ScModule * Agent(ScAddr const & addr)
  {
    static_assert(
        std::is_base_of<ScAgentAbstract, AgentClass>::value, "AgentClass must be derivied from ScAgentAbstract class");
    m_agents.push_back({{GetAgentRegisterCallback<AgentClass>(), GetAgentUnregisterCallback<AgentClass>()}, {addr}});
    return this;
  }

  /*! Registers all module keynodes and agents
   * @returns Result of initializing
   */
  _SC_EXTERN sc_result Register(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr);

  /*! Unregisters all module keynodes and agents
   * @returns Result of shutdown
   */
  _SC_EXTERN sc_result Unregister(ScMemoryContext * ctx);

protected:
  /// Registered keynodes
  std::list<ScKeynodes *> m_keynodes;
  /// Registered agents
  using ScAgentRegisterCallback = std::function<void(ScMemoryContext *, ScAddrVector const &)>;
  using ScAgentUnregisterCallback = std::function<void(ScMemoryContext *)>;
  std::list<std::pair<std::pair<ScAgentRegisterCallback, ScAgentUnregisterCallback>, ScAddrVector>> m_agents;

  template <class AgentClass>
  ScAgentRegisterCallback GetAgentRegisterCallback()
  {
    return [](ScMemoryContext * ctx, ScAddrVector const & addrs)
    {
      AgentClass::template Register<AgentClass>(ctx, addrs);
    };
  }

  template <class AgentClass>
  ScAgentUnregisterCallback GetAgentUnregisterCallback()
  {
    return [](ScMemoryContext * ctx)
    {
      AgentClass::template Unegister<AgentClass>(ctx);
    };
  }
};

/// Implements module class and create it instance
#define SC_MODULE_BODY(__ModuleName__) \
public: \
  static ScModule * m_instance; \
\
  static _SC_EXTERN std::string GetName() \
  { \
    return #__ModuleName__; \
  }

/// Registers module class instance
#define SC_MODULE_REGISTER(__ModuleName__) \
  extern "C" \
  { \
  _SC_EXTERN sc_result \
  sc_module_initialize_with_init_memory_generated_structure(sc_addr const init_memory_generated_structure_addr) \
  { \
    __ModuleName__::m_instance->Register(ScMemory::ms_globalContext, init_memory_generated_structure_addr); \
    return SC_RESULT_OK; \
  } \
\
  _SC_EXTERN sc_result sc_module_shutdown() \
  { \
    __ModuleName__::m_instance->Unregister(ScMemory::ms_globalContext); \
    return SC_RESULT_OK; \
  } \
  } \
  ScModule * __ModuleName__::m_instance = __ModuleName__::Create(new __ModuleName__())

#define SC_MODULE_INITIALIZE(__ModuleName__) \
  sc_result __ModuleName__::Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) \
  { \
    return SC_RESULT_OK; \
  } \
  sc_result __ModuleName__::Shutdown(ScMemoryContext * ctx) \
  { \
    return SC_RESULT_OK; \
  } \
  struct __ModuleName__##Dummy \
  { \
  }