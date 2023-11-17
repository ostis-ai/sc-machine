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

/// Defines `std::string GetName()` method for agent class. It is used in sc-memory logging.
#define SC_AGENT_BODY(__AgentName__) \
  std::string GetName() override \
  { \
    return #__AgentName__; \
  }

/*!
 * @interface An interface for agents classes
 * @note This class is an API to implement your own registration API of agents.
 */
class _SC_EXTERN ScAgentAbstract : public ScObject
{
public:
  SC_AGENT_BODY(ScAgentAbstract)

  _SC_EXTERN explicit ScAgentAbstract(ScEvent::Type const & type, sc_uint8 accessLvl = sc_access_lvl_make_max)
    : m_eventType(type)
    , m_memoryCtx(new ScAgentContext(accessLvl, "ScAgentContext")){};

  _SC_EXTERN ~ScAgentAbstract() override
  {
    m_memoryCtx->Destroy();
    delete m_memoryCtx;
    m_memoryCtx = nullptr;
  }

  /*! @details It will be called, if the event the agent is subscribed to has been initialized. In this method,
   * the logic implemented by the agent starts running. Depending on the event type, the number of valid arguments to
   * this method changes.
   * @param[in] listenAddr An address of a subscripting element (can be empty address)
   * @param[in] edgeAddr An address of an edge element that come in or from subscripting element (can be empty address)
   * @param[in] otherAddr An address of the element incident to the edge (can be empty address)
   * @param[out] answerAddr An answer structure addr
   * @return sc_result A status code of agent on it finish
   */
  _SC_EXTERN virtual sc_result OnEvent(
      ScAddr const & listenAddr,
      ScAddr const & edgeAddr,
      ScAddr const & otherAddr) = 0;

  _SC_EXTERN ScEvent::Type GetEventType()
  {
    return m_eventType;
  }

  /*! Registers agent instance on subscripting addresses
   * @param addrs A vector of subscripting addresses
   * @return A pointer to agent instance
   */
  _SC_EXTERN virtual ScAgentAbstract * Register(ScAddrVector const & addrs) = 0;

  /*! Registers agent instance on subscripting address
   * @param addr A subscripting address
   * @return A pointer to agent instance
   */
  _SC_EXTERN virtual ScAgentAbstract * Register(ScAddr const & addr) = 0;

  /*! Unregisters agent instance on all subscripting address
   * @return A pointer to agent instance
   */
  _SC_EXTERN virtual ScAgentAbstract * Unregister() = 0;

  /*! Complete defined logic if agent is finished successfully (SC_RESULT_OK)
   * @param listenAddr An address of a subscripting element
   * @param edgeAddr An address of an edge element that come in or from subscripting element
   * @param otherAddr An address of the element incident to the edge
   * @return A pointer to agent instance
   */
  _SC_EXTERN virtual void OnSuccess(ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
  {
  }

  /*! Complete defined logic if agent is finished unsuccessfully (SC_RESULT_NO)
   * @param listenAddr An address of a subscripting element
   * @param edgeAddr An address of an edge element that come in or from subscripting element
   * @param otherAddr An address of the element incident to the edge
   * @return A pointer to agent instance
   */
  _SC_EXTERN virtual void OnUnsuccess(ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
  {
  }

  /*! Complete defined logic if agent is finished with error
   * @param listenAddr An addr of a subscripting element
   * @param edgeAddr An addr of an edge element that come in or from subscripting element
   * @param otherAddr An address of the element incident to the edge
   * @param errorCode An error code of agent
   * @return A pointer to agent instance
   */
  _SC_EXTERN virtual void OnError(
      ScAddr const & addr,
      ScAddr const & edgeAddr,
      ScAddr const & otherAddr,
      sc_result errorCode)
  {
  }

protected:
  ScEvent::Type m_eventType = ScEvent::Type::Unknown;
  mutable ScAgentContext * m_memoryCtx;

  std::list<ScEvent *> m_events;

  _SC_EXTERN sc_result Initialize() override
  {
    return SC_RESULT_OK;
  }

  _SC_EXTERN sc_result Initialize(std::string const & initMemoryGeneratedStructure) override
  {
    return SC_RESULT_OK;
  }

  _SC_EXTERN sc_result Shutdown() override
  {
    return SC_RESULT_OK;
  }

  _SC_EXTERN virtual ScEvent::DelegateFunc GetCallback() = 0;
};

/*!
* @class An abstract class to implement agents
* @details The `sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)`
* procedure should be implemented in the child class. You can also override methods `void OnSuccess(ScAddr const &
* listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)`, `void OnUnsuccess(ScAddr const & listenAddr,
* ScAddr const & edgeAddr, ScAddr const & otherAddr)` and `void OnError(ScAddr const & listenAddr, ScAddr const &
* edgeAddr, ScAddr const & otherAddr, sc_result errorCode)`. They are executed depending on the agent error code
* returned in the function `OnEvent`.
* @example
* File sc_syntactic_analysis_agent.hpp:
* \code
* #include "sc-memory/kpm/sc_agent.hpp"
*
* #include "nlp-module/keynodes/sc_nlp_keynodes.hpp"
* #include "nlp-module/agents/sc_syntactic_analysis_agent.hpp"
*
* #include "nlp-module/analyser/sc_syntactic_analyser.hpp"
*
* namespace nlp
* {
*
* template <ScEvent::Type const & type>
* class ScSyntacticAnalysisAgent : public ScAgent<type>
* {
* public:
*   SC_AGENT_BODY(ScSyntacticAnalysisAgent)
*
*   ScSyntacticAnalysisAgent()
*     : m_analyser(new nlp::ScSyntacticAnalyser(m_memoryCtx))
*   {
*   }
*
*   sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override
*   {
*      ScAddr const textAddr = m_memoryCtx->GetArgument(otherAddr, 1);
*      if (textAddr.IsValid() == SC_FALSE)
*      {
*         SC_LOG_ERROR("Invalid text link addr");
*         return SC_RESULT_ERROR_INVALID_PARAMS;
*      }
*
*      try
*      {
*        ScAddr const lexemeTreeAddr = m_analyser->Analyse(textAddr);
*      }
*      catch(utils::ScException const & e)
*      {
*        SC_LOG_ERROR("Error occurred: " + e.Description());
*        return SC_RESULT_ERROR_INVALID_STATE;
*      }
*
*      if (lexemeTreeAddr.IsValid())
*      {
*        SC_LOG_INFO("Lexeme tree has been formed");
*        ScAddr const & answerAddr = m_memoryCtx->FormStructure({ lexemeTreeAddr });
*        m_memoryCtx->FormAnswer(otherAddr, answerAddr);
*        return SC_RESULT_OK;
*      }
*
*      SC_LOG_INFO("Lexeme tree hasn't been formed");
*      return SC_RESULT_NO;
*   }
*
*   void OnSuccess(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override
*   {
*      delete m_memoryCtx->InitializeEvent<ScEvent::Type::AddOutputEdge>(
*          nlp::ScNLPKeynodes::kSyntacticSynthesizeAction,
           [this]() -> {
              ScAddr const & addr = m_memoryCtx->CreateNode(ScType::NodeConst);
              m_memoryCtx->CreateEdge(
                ScType::EdgeAccessConstPosPerm, nlp::ScNLPKeynodes::kSyntacticSynthesizeAction, addr);
           },
           [this](ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) -> sc_result {
              return ms_context->HelperCheckEdge(nlp::ScNLPKeynodes::k, edgeAddr, ScType::EdgeAccessConstPosPerm)
              ? SC_RESULT_OK
              : SC_RESULT_NO;
           }
       )->Wait(10000);
*   }
* };
*
* private:
*   nlp::ScSyntacticAnalyser * m_analyser;
* }
* \endcode
* @note This class is an API to implement your own agent classes.
*/
template <ScEvent::Type const & type>
class _SC_EXTERN ScAgent : public ScAgentAbstract
{
public:
  _SC_EXTERN explicit ScAgent()
    : ScAgentAbstract(type)
  {
  }

  _SC_EXTERN ScAgentAbstract * Register(ScAddrVector const & addrs) override
  {
    for (auto const & addr : addrs)
      m_events.push_back(new ScEvent(*m_memoryCtx, addr, m_eventType, GetCallback()));

    return this;
  }

  _SC_EXTERN ScAgentAbstract * Register(ScAddr const & addr) override
  {
    m_events.push_back(new ScEvent(*m_memoryCtx, addr, m_eventType, GetCallback()));

    return this;
  }

  _SC_EXTERN ScAgentAbstract * Unregister() override
  {
    for (auto * event : m_events)
    {
      delete event;
    }

    m_events.clear();
    return this;
  }

protected:
  _SC_EXTERN ScEvent::DelegateFunc GetCallback() override
  {
    return [this](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr) -> sc_result {
      sc_result result = OnEvent(addr, edgeAddr, otherAddr);

      // finish agent
      if (result == SC_RESULT_OK)
      {
        OnSuccess(addr, edgeAddr, otherAddr);
        m_memoryCtx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::kQuestionFinishedSuccessfully, otherAddr);
      }
      else if (result == SC_RESULT_NO)
      {
        OnUnsuccess(addr, edgeAddr, otherAddr);
        m_memoryCtx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::kQuestionFinishedUnsuccessfully, otherAddr);
      }
      else
      {
        OnError(addr, edgeAddr, otherAddr, result);
        m_memoryCtx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::kQuestionFinishedWithError, otherAddr);
      }
      m_memoryCtx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::kQuestionFinished, otherAddr);

      return result;
    };
  }
};

#define SC_AGENT_REGISTER(__AgentName__, __Addrs__) \
  __AgentName__ __AgentName__##Instance; \
  __AgentName__##Instance.Register(__Addrs__)

#define SC_AGENT_UNREGISTER(__AgentName__) __AgentName__##Instance.Unregister()

#define SC_AGENT_IMPLEMENTATION(__AgentName__) \
  sc_result __AgentName__::OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
