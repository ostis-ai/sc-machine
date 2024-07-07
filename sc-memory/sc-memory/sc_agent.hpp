/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_object.hpp"
#include "sc-memory/sc_keynodes.hpp"

#include "sc_agent_context.hpp"

#include "sc-memory/utils/sc_log.hpp"

#define SC_AGENT_LOG_DEBUG(__msg__) SC_LOG_DEBUG(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_INFO(__msg__) SC_LOG_INFO(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_WARNING(__msg__) SC_LOG_WARNING(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_ERROR(__msg__) SC_LOG_ERROR(GetName() << ": " << __msg__)

/*!
 * @interface An interface for agents classes
 * @note This class is an API to implement your own registration API of agents.
 */
class _SC_EXTERN ScAgentAbstract : public ScObject
{
public:
  _SC_EXTERN ScAgentAbstract();

  _SC_EXTERN ~ScAgentAbstract() override;

  /*! @details It will be called, if the event the agent is subscribed to has been initialized. In this method,
   * the logic implemented by the agent starts running. Depending on the event type, the number of valid arguments to
   * this method changes.
   * @param[in] listenAddr An address of a subscripting element (can be empty address)
   * @param[in] edgeAddr An address of an edge element that come in or from subscripting element (can be empty address)
   * @param[in] otherAddr An address of the element incident to the edge (can be empty address)
   * @return sc_result A status code of agent on it finish
   */
  _SC_EXTERN virtual sc_result OnEvent(
      ScAddr const & listenAddr,
      ScAddr const & edgeAddr,
      ScAddr const & otherAddr) = 0;

protected:
  mutable ScAgentContext m_memoryCtx;

  static std::list<ScEvent *> m_events;

  _SC_EXTERN void SetContext(ScAddr const & userAddr);

  static _SC_EXTERN ScEvent::DelegateFuncWithUserAddr GetCallback();

private:
  _SC_EXTERN sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) override;

  _SC_EXTERN sc_result Shutdown(ScMemoryContext * ctx) override;
};

/*!
 * @interface An interface for implementing agents classes to subscribe its on any sc-events.
 * @note This class is an API to implement your own registration API of agents.
 */
template <ScEvent::Type const & eventType>
class _SC_EXTERN ScAgent : public ScAgentAbstract
{
public:
  /*!
   * @brief Registers an agent class in sc-memory.
   * @tparam TAgentClass Name of agent class being registered. Specified agent class must be derivied from class ScAgent
   * or ScActionAgent.
   * @param ctx Context in which speficied agent class is being registered.
   * @param addrs Enumeration of sc-element addresses to which the specified agent class is subscribed.
   */
  template <class TAgentClass, class... TScAddr>
  static _SC_EXTERN void Register(ScMemoryContext * ctx, TScAddr const &... addrs)
  {
    static_assert(
        std::is_base_of<ScAgentAbstract, TAgentClass>::value,
        "TAgentClass must be derivied from class ScAgent or ScActionAgent.");
    static_assert(std::is_abstract<TAgentClass>::value == false, "TAgentClass must override all virtual functions.");
    static_assert(
        (std::is_base_of<ScAddr, TScAddr>::value && ...),
        "Each argument in the parameter pack must be of class ScAddr.");

    SC_LOG_INFO("Register " << GetName<TAgentClass>());

    for (auto const & addr : ScAddrVector{addrs...})
      m_events.push_back(new ScEvent(*ctx, addr, eventType, TAgentClass::template GetCallback<TAgentClass>()));
  }

  /*!
   * @brief Unregisters an agent class in sc-memory.
   * @tparam TAgentClass Name of agent class being unregistered.
   * @param ctx Сontext in which agent class is being unregistered.
   */
  template <class TAgentClass>
  static _SC_EXTERN void Unregister(ScMemoryContext *)
  {
    static_assert(
        std::is_base_of<ScAgentAbstract, TAgentClass>::value,
        "TAgentClass must be derivied from class ScAgent or ScActionAgent.");
    static_assert(std::is_abstract<TAgentClass>::value == false, "TAgentClass must override all virtual functions.");

    SC_LOG_INFO("Unregister " << GetName<TAgentClass>());

    for (auto * event : m_events)
      delete event;

    m_events.clear();
  }

  /*!
   * @brief Complete defined logic after finishe method OnEvent if agent is finished successfully (SC_RESULT_OK).
   * @param sourceAddr An address of a source sc-element.
   * @param connectorAddr An address of an sc-connector between source and target sc-elements.
   * @param targetAddr An address of a target sc-element.
   * @return A pointer to agent instance.
   */
  _SC_EXTERN virtual void OnSuccess(
      ScAddr const & /* sourceAddr */,
      ScAddr const & /* connectorAddr */,
      ScAddr const & /* targetAddr */)
  {
  }

  /*!
   * @brief Complete defined logic after finished method OnEvent if agent is finished unsuccessfully (SC_RESULT_NO).
   * @param sourceAddr An address of a source sc-element.
   * @param connectorAddr An address of an sc-connector between source and target sc-elements.
   * @param targetAddr An address of a target sc-element.
   * @return A pointer to agent instance.
   */
  _SC_EXTERN virtual void OnUnsuccess(
      ScAddr const & /* sourceAddr */,
      ScAddr const & /* connectorAddr */,
      ScAddr const & /* targetAddr */)
  {
  }

  /*!
   * @brief Complete defined logic after finished method OnEvent if agent is finished with error.
   * @param sourceAddr An address of a source sc-element.
   * @param connectorAddr An address of an sc-connector between source and target sc-elements.
   * @param targetAddr An address of a target sc-element.
   * @param errorCode An error code of agent.
   * @return A pointer to agent instance.
   */
  _SC_EXTERN virtual void OnError(
      ScAddr const & /* sourceAddr */,
      ScAddr const & /* connectorAddr */,
      ScAddr const & /* targetAddr */,
      sc_result /* result */)
  {
  }

protected:
  _SC_EXTERN explicit ScAgent()
    : ScAgentAbstract(){};

  template <class TAgentClass>
  static _SC_EXTERN ScEvent::DelegateFuncWithUserAddr GetCallback()
  {
    return [](ScAddr const & userAddr,
              ScAddr const & sourceAddr,
              ScAddr const & connectorAddr,
              ScType const &,
              ScAddr const & targetAddr) -> sc_result
    {
      TAgentClass agent;
      agent.SetContext(userAddr);

      SC_LOG_INFO(agent.GetName() << " started");
      sc_result const result = agent.OnEvent(sourceAddr, connectorAddr, targetAddr);

      // finish agent
      if (result == SC_RESULT_OK)
      {
        SC_LOG_INFO(agent.GetName() << " finished successfully");
        agent.OnSuccess(sourceAddr, connectorAddr, targetAddr);
      }
      else if (result == SC_RESULT_NO)
      {
        SC_LOG_INFO(agent.GetName() << " finished unsuccessfully");
        agent.OnUnsuccess(sourceAddr, connectorAddr, targetAddr);
      }
      else
      {
        SC_LOG_INFO(agent.GetName() << " finished with error");
        agent.OnError(sourceAddr, connectorAddr, targetAddr, result);
      }

      return result;
    };
  }
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
 *  * It uses the `SC_AGENT_BODY` macro and has a member `m_analyser` of type `nlp::ScSyntacticAnalyser`.
 *  * The main logic is implemented in the `OnEvent` method.
 *  *
 * class ScSyntacticAnalysisAgent : public ScActionAgent<ScKeynodes::kSyntacticAnalyseAction>
 * {
 * public:
 *   ScSyntacticAnalysisAgent();
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
 *   ScAddr const & answerAddr = m_memoryCtx->FormStructure({lexemeTreeAddr});
 *   m_memoryCtx.FormAnswer(actionAddr, answerAddr);
 *   return SC_RESULT_OK;
 * }
 *
 * void ScSyntacticAnalysisAgent::OnSuccess(ScAddr const & actionAddr)
 * {
 *   delete m_memoryCtx->InitializeEvent<ScEvent::Type::AddOutputEdge>(
 *   nlp::ScNLPKeynodes::kSyntacticSynthesizeAction,
 *     [this]() -> {
 *       ScAddr const & addr = m_memoryCtx.CreateNode(ScType::NodeConst);
 *       m_memoryCtx.CreateEdge(
 *         ScType::EdgeAccessConstPosPerm, nlp::ScNLPKeynodes::kSyntacticSynthesizeAction, addr);
 *     },
 *     [this](ScAddr const &, ScAddr const &, ScAddr const & actionAddr) -> sc_result {
 *       return m_memoryCtx.HelperCheckEdge(nlp::ScNLPKeynodes::kActionFinishedSucessfully, actionAddr,
 ScType::EdgeAccessConstPosPerm)
 *         ? SC_RESULT_OK
 *         : SC_RESULT_NO;
 *     }
 *   )->Wait(10000);
 * }
 * \endcode
 */
template <ScKeynodeClass const & actionClass = ScKeynodes::empty_class>
class _SC_EXTERN ScActionAgent : public ScAgent<ScEvent::Type::AddOutputEdge>
{
public:
  _SC_EXTERN explicit ScActionAgent()
    : ScAgent()
  {
  }

  /*!
   * @details This method is called when the event the agent is subscribed to is initialized.
   * It implements the main logic of the agent.
   * @param[in] actionAddr An address of the action sc-element.
   * @return sc_result A status code indicating the result of the agent's execution.
   */
  _SC_EXTERN virtual sc_result OnEvent(ScAddr const & actionAddr) = 0;

  /*!
   * @brief Completes defined logic if the agent finishes successfully (SC_RESULT_OK).
   * @param actionAddr An address of the action sc-element.
   */
  _SC_EXTERN virtual void OnSuccess(ScAddr const & /* actionAddr */) {}

  /*!
   * @brief Completes defined logic if the agent finishes unsuccessfully (SC_RESULT_NO).
   * @param actionAddr An address of the action sc-element.
   */
  _SC_EXTERN virtual void OnUnsuccess(ScAddr const & /* actionAddr */) {}

  /*!
   * @brief Completes defined logic if the agent finishes with an error.
   * @param actionAddr An address of the action element.
   * @param errorCode An error code indicating the type of error.
   */
  _SC_EXTERN virtual void OnError(ScAddr const & /* actionAddr */, sc_result /* result */) {}

  /*!
   * @brief Provides a callback function for handling events in the sc-memory.
   *
   * @tparam TAgentClass The class of the agent that will handle the event.
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
  template <class TAgentClass>
  static _SC_EXTERN ScEvent::DelegateFuncWithUserAddr GetCallback()
  {
    return [](ScAddr const & userAddr,
              ScAddr const &,
              ScAddr const &,
              ScType const & connectorType,
              ScAddr const & actionAddr) -> sc_result
    {
      sc_result result = SC_RESULT_ERROR;
      if (!(connectorType.BitAnd(ScType::EdgeAccess) == ScType::EdgeAccess)
          || (actionClass.IsValid()
              && !ScMemory::ms_globalContext->HelperCheckEdge(actionClass, actionAddr, ScType::EdgeAccessConstPosPerm)))
        return result;

      TAgentClass agent;
      agent.SetContext(userAddr);
      SC_LOG_INFO(agent.GetName() << " started");
      result = agent.OnEvent(actionAddr);

      // finish agent
      if (result == SC_RESULT_OK)
      {
        SC_LOG_INFO(agent.GetName() << " finished successfully");
        agent.OnSuccess(actionAddr);
        ScMemory::ms_globalContext->CreateEdge(
            ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished_successfully, actionAddr);
      }
      else if (result == SC_RESULT_NO)
      {
        SC_LOG_INFO(agent.GetName() << " finished unsuccessfully");
        agent.OnUnsuccess(actionAddr);
        ScMemory::ms_globalContext->CreateEdge(
            ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished_unsuccessfully, actionAddr);
      }
      else
      {
        SC_LOG_INFO(agent.GetName() << " finished with error");
        agent.OnError(actionAddr, result);
        ScMemory::ms_globalContext->CreateEdge(
            ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished_with_error, actionAddr);
      }
      ScMemory::ms_globalContext->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished, actionAddr);

      return result;
    };
  }

private:
  _SC_EXTERN _SC_DELETED_FUNCTION sc_result OnEvent(ScAddr const &, ScAddr const &, ScAddr const &) final
  {
    return SC_RESULT_OK;
  }

  _SC_EXTERN _SC_DELETED_FUNCTION void OnSuccess(ScAddr const &, ScAddr const &, ScAddr const &) final {}

  _SC_EXTERN _SC_DELETED_FUNCTION void OnUnsuccess(ScAddr const &, ScAddr const &, ScAddr const &) final {}

  _SC_EXTERN _SC_DELETED_FUNCTION void OnError(ScAddr const &, ScAddr const &, ScAddr const &, sc_result) final {}
};

/*!
 * @brief Registers an agent class in sc-memory.
 * @tparam TAgentClass Name of agent class being registered. Specified agent class must be derivied from class ScAgent
 * or ScActionAgent.
 * @param ctx Context in which speficied agent class is being registered.
 * @param addrs Enumeration of sc-element addresses to which the specified agent class is subscribed.
 */
template <class TAgentClass, class... TScAddr>
_SC_EXTERN void RegisterAgent(ScMemoryContext * ctx, TScAddr const &... addrs)
{
  TAgentClass::template Register<TAgentClass, TScAddr...>(ctx, addrs...);
}

/*!
 * @brief Unregisters an agent class in sc-memory.
 * @tparam TAgentClass Name of agent class being unregistered.
 * @param ctx Сontext in which agent class is being unregistered.
 */
template <class TAgentClass>
_SC_EXTERN void UnregisterAgent(ScMemoryContext * ctx)
{
  TAgentClass::template Unregister<TAgentClass>(ctx);
}
