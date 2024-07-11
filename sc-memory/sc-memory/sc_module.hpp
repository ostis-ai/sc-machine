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
 * #pragma once
 *
 * #include "sc-memory/sc_module.hpp"
 *
 * class ScNLPModule final : public ScModule
 * {
 * };
 *
 * \endcode
 * File sc_nlp_module.cpp:
 * \code
 * #include "nlp-module/sc_nlp_module.hpp"
 *
 * #include "nlp-module/keynodes/sc_nlp_keynodes.hpp"
 * #include "nlp-module/agents/sc_syntactic_analysis_agent.hpp"
 * #include "nlp-module/agents/sc_semantic_analysis_agent.hpp"
 *
 * SC_MODULE_REGISTER(ScNLPModule)
 *   ->Keynodes<nlp::ScNLPKeynodes>()
 *   ->Agent<nlp::ScSyntacticAnalysisAgent>()
 *   ->Agent<nlp::ScSemanticAnalysisAgent>();
 *
 * \endcode
 * @note Not recommended to use interface API to implement module classes. Use example defines instead of.
 */
class _SC_EXTERN ScModule : public ScObject
{
public:
  _SC_EXTERN ~ScModule() override = default;

  _SC_EXTERN static ScModule * Create(ScModule * module)
  {
    return module;
  }

  /*! Reminds keynodes to register it with module after.
   * @param keynodes A pointer to dynamically created keynodes instance
   * @returns Pointer to module instance
   */
  template <class TKeynodesClass>
  _SC_EXTERN ScModule * Keynodes()
  {
    static_assert(
        std::is_base_of<ScKeynodes, TKeynodesClass>::value, "TKeynodesClass must be derivied from ScKeynodes class.");
    m_keynodes.push_back(new TKeynodesClass());
    return this;
  }

  /*! Reminds agent and it initiation condition to register it with module after.
   * @param agentInfo A pointer to dynamically created agent instance and a vector of subscription addrs
   * @returns Pointer to module instance
   */
  template <class TAgentClass, class... TScAddr>
  _SC_EXTERN ScModule * Agent(TScAddr const &... addrs)
  {
    static_assert(
        std::is_base_of<ScAgentAbstract, TAgentClass>::value,
        "TAgentClass must be derivied from class ScAgent or ScActionAgent.");
    static_assert(std::is_abstract<TAgentClass>::value == false, "TAgentClass must override all virtual functions.");
    static_assert(
        (std::is_base_of<ScAddr, TScAddr>::value && ...),
        "Each argument in the parameter pack must be of class ScAddr.");

    m_agents.push_back(
        {{GetAgentRegisterCallback<TAgentClass>(), GetAgentUnregisterCallback<TAgentClass>()}, {addrs...}});
    return this;
  }

  /*! Registers all module keynodes and agents
   * @returns Result of initializing
   */
  _SC_EXTERN sc_result Register(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr = ScAddr::Empty);

  /*! Unregisters all module keynodes and agents
   * @returns Result of shutdown
   */
  _SC_EXTERN sc_result Unregister(ScMemoryContext * ctx);

  _SC_EXTERN sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) override;

  _SC_EXTERN sc_result Shutdown(ScMemoryContext * ctx) override;

protected:
  /// Registered keynodes
  std::list<ScKeynodes *> m_keynodes;
  /// Registered agents
  using ScAgentRegisterCallback = std::function<void(ScMemoryContext *, ScAddrVector const &)>;
  using ScAgentUnregisterCallback = std::function<void(ScMemoryContext *)>;
  std::list<std::pair<std::pair<ScAgentRegisterCallback, ScAgentUnregisterCallback>, ScAddrVector>> m_agents;

  template <class TAgentClass>
  ScAgentRegisterCallback GetAgentRegisterCallback()
  {
    return [](ScMemoryContext * ctx, ScAddrVector const &)
    {
      TAgentClass::template Register<TAgentClass>(ctx);
    };
  }

  template <class TAgentClass>
  ScAgentUnregisterCallback GetAgentUnregisterCallback()
  {
    return [](ScMemoryContext * ctx)
    {
      TAgentClass::template Unregister<TAgentClass>(ctx);
    };
  }
};

/// Registers module class instance
#define SC_MODULE_REGISTER(__ModuleName__) \
  struct __ModuleName__##Dummy \
  { \
    static ScModule * ms_module; \
  }; \
  extern "C" \
  { \
  _SC_EXTERN sc_result \
  sc_module_initialize_with_init_memory_generated_structure(sc_addr const init_memory_generated_structure_addr) \
  { \
    __ModuleName__##Dummy::ms_module->Register(ScMemory::ms_globalContext, init_memory_generated_structure_addr); \
    return SC_RESULT_OK; \
  } \
\
  _SC_EXTERN sc_result sc_module_shutdown() \
  { \
    __ModuleName__##Dummy::ms_module->Unregister(ScMemory::ms_globalContext); \
    return SC_RESULT_OK; \
  } \
  } \
  ScModule * __ModuleName__##Dummy::ms_module = __ModuleName__::Create(new __ModuleName__())
