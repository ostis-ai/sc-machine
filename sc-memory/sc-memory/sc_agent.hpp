/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "sc_event.hpp"
#include "sc_event_subscription.hpp"
#include "sc_action.hpp"

#include "sc_agent_context.hpp"

#include "utils/sc_log.hpp"

#define SC_AGENT_LOG_DEBUG(__msg__) SC_LOG_DEBUG(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_INFO(__msg__) SC_LOG_INFO(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_WARNING(__msg__) SC_LOG_WARNING(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_ERROR(__msg__) SC_LOG_ERROR(GetName() << ": " << __msg__)

/*!
 * @interface An interface for agents classes
 * @note This class is an API to implement your own registration API of agents.
 */
template <ScEventClass TScEvent>
class _SC_EXTERN ScAgentAbstract : public ScObject
{
public:
  _SC_EXTERN ScAgentAbstract();

  _SC_EXTERN ~ScAgentAbstract() override;

  /*! @details It will be called, if the event the agent is subscribed to has been initialized. In this method,
   * the logic implemented by the agent starts running. Depending on the event type, the number of valid arguments to
   * this method changes.
   * @return sc_result A status code of agent on it finish
   */
  _SC_EXTERN virtual sc_result OnEvent(TScEvent const & event) = 0;

  /*!
   * @brief Complete defined logic after finishe method OnEvent if agent is finished successfully (SC_RESULT_OK).
   * @return A pointer to agent instance.
   */
  _SC_EXTERN virtual void OnSuccess(TScEvent const & /* event */) {}

  /*!
   * @brief Complete defined logic after finished method OnEvent if agent is finished unsuccessfully (SC_RESULT_NO).
   * @return A pointer to agent instance.
   */
  _SC_EXTERN virtual void OnUnsuccess(TScEvent const & /* event */) {}

  /*!
   * @brief Complete defined logic after finished method OnEvent if agent is finished with error.
   * @return A pointer to agent instance.
   */
  _SC_EXTERN virtual void OnError(TScEvent const & /* event */, sc_result /* result */) {}

protected:
  mutable ScAgentContext m_memoryCtx;

  static inline std::
      unordered_map<ScAddr, ScElementaryEventSubscription<TScEvent> *, ScAddrHashFunc<sc_uint32>, ScAddrLessFunc>
          m_events;

  _SC_EXTERN void SetContext(ScAddr const & userAddr);

  static _SC_EXTERN std::function<sc_result(TScEvent const &)> GetCallback();

private:
  _SC_EXTERN sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) override;

  _SC_EXTERN sc_result Shutdown(ScMemoryContext *) override;
};

template <class TScAgent, class TScEvent>
concept ScAgentClass = requires {
  requires std::derived_from<TScAgent, ScAgentAbstract<TScEvent>>;
  requires !std::is_abstract_v<TScAgent>;
};

/*!
 * @interface An interface for implementing agents classes to subscribe its on any sc-events.
 * @note This class is an API to implement your own registration API of agents.
 */
template <ScEventClass TScEvent>
class _SC_EXTERN ScAgent : public ScAgentAbstract<TScEvent>
{
public:
  /*!
   * @brief Registers an agent class in sc-memory.
   * @tparam TScAgent Name of agent class being registered. Specified agent class must be derivied from class ScAgent
   * or ScActionAgent.
   * @param ctx Context in which speficied agent class is being registered.
   */
  template <ScAgentClass<TScEvent> TScAgent, ScAddrClass... TScAddr>
  static _SC_EXTERN void Register(ScMemoryContext * ctx, TScAddr const &... subscriptionAddrs);

  /*!
   * @brief Unregisters an agent class in sc-memory.
   * @tparam TScAgent Name of agent class being unregistered.
   * @param ctx Сontext in which agent class is being unregistered.
   */
  template <ScAgentClass<TScEvent> TScAgent, ScAddrClass... TScAddr>
  static _SC_EXTERN void Unregister(ScMemoryContext *, TScAddr const &... subscriptionAddrs);

protected:
  _SC_EXTERN explicit ScAgent();

  template <ScAgentClass<TScEvent> TScAgent>
  static _SC_EXTERN std::function<sc_result(TScEvent const &)> GetCallback();
};

/*!
 * @interface This interface is an API to implement your own agent classes interpreting action classes.
 * @details The `sc_result OnEvent(ScAddr const & actionAddr)` procedure should be implemented in the child class.
 * You can also override methods `void OnSuccess(ScAddr const & actionAddr)`, `void OnUnsuccess(ScAddr const &
 * actionAddr)` and `void OnError(ScAddr const & actionAddr, sc_result errorCode)`.
 * They are executed depending on the agent error code returned in the function `OnEvent`.
 *
 * File sc_syntactic_analysis_agent.hpp:
 * \code
 * #pragma once
 *
 * #include <sc-memory/sc_agent.hpp>
 *
 * #include "nlp-module/keynodes/sc_nlp_keynodes.hpp"
 *
 * #include "nlp-module/analyser/sc_syntactic_analyser.hpp"
 *
 * namespace nlp
 * {
 *  *!
 *  * @details The `ScSyntacticAnalysisAgent` class inherits from `ScAgent` and overrides the `OnEvent` and `OnSuccess`
 *  * methods.
 *  * It has a member `m_analyser` of type `nlp::ScSyntacticAnalyser`.
 *  * The main logic is implemented in the `OnEvent` method.
 *  *
 * class ScSyntacticAnalysisAgent : public ScActionAgent<ScKeynodes::syntactic_analyse_action>
 * {
 * public:
 *   ScSyntacticAnalysisAgent();
 *
 *   ~ScSyntacticAnalysisAgent();
 *
 *   sc_result OnEvent(ScAddr const & actionAddr) override;
 *
 *   void OnSuccess(ScAddr const & actionAddr) override;
 * };
 *
 * private:
 *   nlp::ScSyntacticAnalyser * m_analyser;
 * }
 * \endcode
 *
 * File sc_syntactic_analysis_agent.cpp:
 * \code
 * #include "nlp-module/agents/sc_syntactic_analysis_agent.hpp"
 *
 * ScSyntacticAnalysisAgent::ScSyntacticAnalysisAgent()
 *   : m_analyser(new nlp::ScSyntacticAnalyser(m_memoryCtx))
 * {
 * }
 *
 * ScSyntacticAnalysisAgent::~ScSyntacticAnalysisAgent()
 * {
 *   delete m_analyser;
 * }
 *
 * sc_result ScSyntacticAnalysisAgent::OnEvent(ScAddr const & actionAddr)
 * {
 *   ScAddr const textAddr = m_memoryCtx.GetArgument(actionAddr, 1);
 *   if (!textAddr.IsValid())
 *   {
 *     SC_AGENT_LOG_ERROR("Invalid text link addr");
 *     return SC_RESULT_ERROR_INVALID_PARAMS;
 *   }
 *
 *   try
 *   {
 *     ScAddr const lexemeTreeAddr = m_analyser->Analyse(textAddr);
 *   }
 *   catch(utils::ScException const & e)
 *   {
 *     SC_AGENT_LOG_ERROR("Error occurred: " << e.Message());
 *     return SC_RESULT_ERROR_INVALID_STATE;
 *   }
 *
 *   if (!lexemeTreeAddr.IsValid())
 *   {
 *     SC_AGENT_LOG_INFO("Lexeme tree hasn't been formed");
 *     return SC_RESULT_NO;
 *   }
 *
 *   SC_AGENT_LOG_INFO("Lexeme tree has been formed");
 *   ScAddr const & answerAddr = m_memoryCtx->FormStructure(lexemeTreeAddr);
 *   m_memoryCtx.FormAnswer(actionAddr, answerAddr);
 *   return SC_RESULT_OK;
 * }
 *
 * void ScSyntacticAnalysisAgent::OnSuccess(ScAddr const & actionAddr)
 * {
 *   delete m_memoryCtx->InitializeEvent<ScEvent::Type::AddOutputEdge>(
 *   nlp::ScNLPKeynodes::syntactic_synthesize_action,
 *     [this]() -> {
 *       ScAddr const & addr = m_memoryCtx.CreateNode(ScType::NodeConst);
 *       m_memoryCtx.CreateEdge(
 *         ScType::EdgeAccessConstPosPerm, nlp::ScNLPKeynodes::syntactic_synthesize_action, addr);
 *     },
 *     [this](ScAddr const &, ScAddr const &, ScAddr const & actionAddr) -> sc_result {
 *       return m_memoryCtx.HelperCheckEdge(nlp::ScNLPKeynodes::action_finished_sucessfully, actionAddr,
 ScType::EdgeAccessConstPosPerm)
 *         ? SC_RESULT_OK
 *         : SC_RESULT_NO;
 *     }
 *   )->Wait(10000);
 * }
 * \endcode
 */
class _SC_EXTERN ScActionAgent : public ScAgent<ScActionEvent>
{
public:
  /*!
   * @brief Registers an agent class in sc-memory.
   * @tparam TScAgent Name of agent class being registered. Specified agent class must be derivied from class ScAgent
   * or ScActionAgent.
   * @param ctx Context in which speficied agent class is being registered.
   */
  template <ScAgentClass<ScActionEvent> TScAgent>
  static _SC_EXTERN void Register(ScMemoryContext * ctx, ScAddr const & actionClassAddr);

  /*!
   * @brief Unregisters an agent class in sc-memory.
   * @tparam TScAgent Name of agent class being unregistered.
   * @param ctx Сontext in which agent class is being unregistered.
   */
  template <ScAgentClass<ScActionEvent> TScAgent>
  static _SC_EXTERN void Unregister(ScMemoryContext *, ScAddr const & actionClassAddr);

protected:
  /*!
   * @brief Provides a callback function for handling events in the sc-memory.
   *
   * @tparam TScAgent The class of the agent that will handle the event.
   * @return ScEvent::DelegateFuncWithUserAddr A delegate function that processes the event.
   *
   * @details This function template returns a lambda function that serves as a callback for event handling.
   *          The callback function:
   *          - Checks if initiated action has specified action class for this agent.
   *          - Creates an instance of the specified agent class.
   *          - Calls the `OnEvent` method of the agent to handle the event.
   *          - Depending on the result of the `OnEvent` method, it calls `OnSuccess`, `OnUnsuccess`, or `OnError`
   * methods of the agent.
   *          - Logs the status of the agent's execution and creates corresponding edges in the sc-memory.
   */
  template <class TScAgent>
  static _SC_EXTERN std::function<sc_result(ScActionEvent const &)> GetCallback(ScAddr const & actionClassAddr);
};

#include "sc_agent.tpp"

/*!
 * @brief Registers an agent class in sc-memory.
 * @tparam TAgentClass Name of agent class being registered. Specified agent class must be derivied from class ScAgent
 * or ScActionAgent.
 * @param ctx Context in which speficied agent class is being registered.
 */
template <class TAgentClass, ScAddrClass... TScAddr>
_SC_EXTERN void RegisterAgent(ScMemoryContext * ctx, TScAddr const &... subscriptionAddrs)
{
  TAgentClass::template Register<TAgentClass>(ctx, subscriptionAddrs...);
}

/*!
 * @brief Unregisters an agent class in sc-memory.
 * @tparam TAgentClass Name of agent class being unregistered.
 * @param ctx Сontext in which agent class is being unregistered.
 */
template <class TAgentClass, ScAddrClass... TScAddr>
_SC_EXTERN void UnregisterAgent(ScMemoryContext * ctx, TScAddr const &... subscriptionAddrs)
{
  TAgentClass::template Unregister<TAgentClass>(ctx, subscriptionAddrs...);
}
