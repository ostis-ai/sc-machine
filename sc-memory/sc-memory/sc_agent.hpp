/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "sc_agent_context.hpp"
#include "sc_agent_builder.hpp"

#include "utils/sc_log.hpp"

//! Log functions to be used in agent class methods.
#define SC_AGENT_LOG_DEBUG(__msg__) SC_LOG_DEBUG(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_INFO(__msg__) SC_LOG_INFO(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_WARNING(__msg__) SC_LOG_WARNING(GetName() << ": " << __msg__)
#define SC_AGENT_LOG_ERROR(__msg__) SC_LOG_ERROR(GetName() << ": " << __msg__)

template <class TScEvent>
class TScElementaryEventSubscription;
class ScAction;
class ScResult;

/*!
 * @class ScAgentAbstract
 * @brief An abstract base class for sc-agents.
 *
 * This class provides a base implementation for sc-agents, offering methods for initialization, shutdown, and handling
 * events.
 *
 *
 * @tparam TScEvent The type of sc-event this agent handles.
 */
template <class TScEvent>
class _SC_EXTERN ScAgentAbstract : public ScObject
{
  static_assert(std::is_base_of<class ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

public:
  _SC_EXTERN ~ScAgentAbstract() override;

  _SC_EXTERN virtual ScAddr GetAbstractAgent() const;

  _SC_EXTERN virtual ScAddr GetEventClass() const;

  _SC_EXTERN virtual ScAddr GetEventSubscriptionElement() const;

  _SC_EXTERN virtual ScAddr GetActionClass() const;

  /*!
   * @brief Checks the initiation condition for agent of this class.
   *
   * @param event The sc-event to check.
   * @return ScTemplate of action initiation condition.
   */
  _SC_EXTERN virtual sc_bool CheckInitiationCondition(TScEvent const & event);

  _SC_EXTERN virtual ScAddr GetInitiationCondition() const;

  _SC_EXTERN virtual ScTemplate GetInitiationConditionTemplate() const;

  _SC_EXTERN virtual ScResult DoProgram(TScEvent const & event, ScAction & action) = 0;

  /*!
   * @brief Gets the result of the agent's execution.
   *
   * @param event The sc-event.
   * @param action The sc-action.
   * @return ScTemplate of sucessfully finished action result condition.
   */
  _SC_EXTERN virtual sc_bool CheckResultCondition(TScEvent const & event, ScAction & action);

  _SC_EXTERN virtual ScAddr GetResultCondition() const;

  _SC_EXTERN virtual ScTemplate GetResultConditionTemplate() const;

protected:
  mutable ScAgentContext m_memoryCtx;
  ScAddr m_agentImplementationAddr;

  static inline std::
      unordered_map<std::string, std::unordered_map<ScAddr, ScElementaryEventSubscription *, ScAddrHashFunc>>
          m_events;

  _SC_EXTERN ScAgentAbstract();

  _SC_EXTERN void SetContext(ScAddr const & userAddr);

  _SC_EXTERN void SetImplementation(ScAddr const & agentImplementationAddr);

  _SC_EXTERN sc_bool MayBeSpecified() const;

  static _SC_EXTERN std::function<void(TScEvent const &)> GetCallback(ScAddr const & agentImplementationAddr);
};

/*!
 * @class ScAgent
 * @brief A class for sc-agents that can subscribe to events.
 *
 * This class extends ScAgentAbstract and provides methods for subscribing and unsubscribing
 * to sc-events.
 *
 * @tparam TScEvent The type of sc-event this agent class handles.
 */
template <class TScEvent>
class _SC_EXTERN ScAgent : public ScAgentAbstract<TScEvent>
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

public:
  /*!
   * @brief Subscribes agent class to specified sc-events.
   *
   * @tparam TScAgent An agent class to be subscribed to the event. Specified agent class must be derivied from class
   * ScAgent.
   * @param ctx A sc-memory context.
   * @param subscriptionAddrs A list of sc-addresses of sc-elements to subscribe to.
   */
  template <class TScAgent, class... TScAddr>
  static _SC_EXTERN void Subscribe(
      ScMemoryContext * ctx,
      ScAddr const & agentImplementationAddr,
      TScAddr const &... subscriptionAddrs);

  /*!
   * @brief Unsubscribes agent class from specified sc-events.
   *
   * @tparam TScAgent An agent class to be subscribed to the event. Specified agent class must be derivied from class
   * ScAgent.
   * @param ctx A sc-memory context.
   * @param subscriptionAddrs A list of sc-addresses of sc-elements to unsubscribe from.
   */
  template <class TScAgent, class... TScAddr>
  static _SC_EXTERN void Unsubscribe(
      ScMemoryContext * ctx,
      ScAddr const & agentImplementationAddr,
      TScAddr const &... subscriptionAddrs);

protected:
  _SC_EXTERN explicit ScAgent();

  /*!
   * @brief Gets the callback function for agent class.
   *
   * @tparam TScAgent An agent class to be subscribed to the event. Specified agent class must be derivied from class
   * ScAgent.
   * @return A function that takes an sc-event and returns an sc-result.
   */
  template <class TScAgent>
  static _SC_EXTERN std::function<void(TScEvent const &)> GetCallback(ScAddr const & agentImplementationAddr);
};

/*!
 * @class ScActionAgent
 * @brief A specialized agent class for handling sc-actions.
 *
 * This class extends ScAgent and provides methods for subscribing and unsubscribing
 * to sc-action events.
 *
 * @details The `ScResult DoProgram(ScActionEvent const & event, ScAction & action)` procedure should be
 * implemented in the child class.
 *
 * File sc_syntactic_analysis_agent.hpp:
 * \code
 * #pragma once
 *
 * #include <sc-memory/sc_agent.hpp>
 *
 * #include "nlp-module/analyser/sc_syntactic_analyser.hpp"
 *
 * namespace nlp
 * {
 *  *!
 *  * @details The `ScSyntacticAnalysisAgent` class inherits from `ScAgent` and overrides the `DoProgram`.
 *  * It has a member `m_analyser` of type `nlp::ScSyntacticAnalyser`.
 *  * The main logic is implemented in the `DoProgram` method.
 *  *
 * class ScSyntacticAnalysisAgent : public ScActionAgent
 * {
 * public:
 *   ScSyntacticAnalysisAgent();
 *
 *   ~ScSyntacticAnalysisAgent();
 *
 *   ScAddr GetActionClass() const override;
 *
 *   ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
 * };
 *
 * private:
 *   ScSyntacticAnalyser * m_analyser;
 * }
 * \endcode
 *
 * File sc_syntactic_analysis_agent.cpp:
 * \code
 * #include "nlp-module/agents/sc_syntactic_analysis_agent.hpp"
 *
 * #include "nlp-module/keynodes/sc_nlp_keynodes.hpp"
 *
 * ScSyntacticAnalysisAgent::ScSyntacticAnalysisAgent()
 *   : m_analyser(new ScSyntacticAnalyser(m_memoryCtx))
 * {
 * }
 *
 * ScSyntacticAnalysisAgent::~ScSyntacticAnalysisAgent()
 * {
 *   delete m_analyser;
 * }
 *
 * ScAddr ScSyntacticAnalysisAgent::GetActionClass() const
 * {
 *   return ScNLPKeynodes::action_syntactic_analysis;
 * }
 *
 * ScResult ScSyntacticAnalysisAgent::DoProgram(ScActionEvent const &, ScAction & action)
 * {
 *   ScAddr const textAddr = action.GetArgument(1);
 *   if (!textAddr.IsValid())
 *   {
 *     SC_AGENT_LOG_ERROR("Text link is not specified as the first argument.");
 *     return action.FinishWithError();
 *   }
 *
 *   try
 *   {
 *     ScAddr const lexemeTreeAddr = m_analyser->Analyse(textAddr);
 *   }
 *   catch (utils::ScException const & e)
 *   {
 *     SC_AGENT_LOG_ERROR("Error occurred: " << e.Message());
 *     return action.FinishWithError();
 *   }
 *
 *   if (!lexemeTreeAddr.IsValid())
 *   {
 *     SC_AGENT_LOG_INFO("Lexeme tree hasn't been formed");
 *     return action.FinishUnsuccessfully();
 *   }
 *
 *   SC_AGENT_LOG_INFO("Lexeme tree has been formed");
 *   action.SetAnswer(answerAddr);
 *   return action.FinishSuccessfully();
 * }
 * \endcode
 */

class _SC_EXTERN ScActionAgent : public ScAgent<ScActionEvent>
{
public:
  /*!
   * @brief Subscribes agent class to sc-event of adding output arc from `action_initiated` to some formed sc-action.
   *
   * @tparam TScAgent An agent class to be subscribed to the event. Specified agent class must be derivied from class
   * ScAgentAction.
   * @param ctx A sc-memory context.
   */
  template <class TScAgent>
  static _SC_EXTERN void Subscribe(ScMemoryContext * ctx, ScAddr const & agentImplementationAddr);

  /*!
   * @brief Unsubscribes agent class from sc-event of adding output arc from `action_initiated` to some formed
   * sc-action.
   *
   * @tparam TScAgent An agent class to be subscribed to the event. Specified agent class must be derivied from class
   * ScAgentAction.
   * @param ctx A sc-memory context.
   */
  template <class TScAgent>
  static _SC_EXTERN void Unsubscribe(ScMemoryContext * ctx, ScAddr const & agentImplementationAddr);

  /*!
   * @brief Checks that initiated sc-action belongs to action class that this agent class interpreters.
   *
   * @param event The sc-event to check.
   * @return SC_TRUE if the condition is met, otherwise SC_FALSE.
   */
  _SC_EXTERN ScTemplate GetInitiationConditionTemplate() const override;

protected:
  ScActionAgent();
};

#include "sc_agent.tpp"

/*!
 * @brief Subscribes agent class to specified sc-events.
 *
 * @tparam TScAgent An agent class to be subscribed to the event. Specified agent class must be derivied from class
 * ScAgent.
 * @param ctx A sc-memory context.
 * @param subscriptionAddrs A list of sc-addresses of sc-elements to subscribe to.
 */
template <class TScAgent, class... TScAddr>
typename std::enable_if<!std::is_base_of<ScActionAgent, TScAgent>::value>::type SubscribeAgent(
    ScMemoryContext * ctx,
    TScAddr const &... subscriptionAddrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent::template Subscribe<TScAgent>(ctx, ScAddr::Empty, subscriptionAddrs...);
}

/*!
 * @brief Subscribes agent class to sc-event of adding output arc from `action_initiated` to some formed sc-action.
 *
 * @tparam TScAgent An agent class to be subscribed to the event. Specified agent class must be derivied from class
 * ScAgentAction.
 * @param ctx A sc-memory context.
 */
template <class TScAgent>
typename std::enable_if<std::is_base_of<ScActionAgent, TScAgent>::value>::type SubscribeAgent(ScMemoryContext * ctx)
{
  ScActionAgent::template Subscribe<TScAgent>(ctx, ScAddr::Empty);
}

/*!
 * @brief Unsubscribes agent class from specified sc-events.
 *
 * @tparam TScAgent An agent class to be subscribed to the event. Specified agent class must be derivied from class
 * ScAgent.
 * @param ctx A sc-memory context.
 * @param subscriptionAddrs A list of sc-addresses of sc-elements to subscribe from.
 */
template <class TScAgent, class... TScAddr>
typename std::enable_if<!std::is_base_of<ScActionAgent, TScAgent>::value>::type UnsubscribeAgent(
    ScMemoryContext * ctx,
    TScAddr const &... subscriptionAddrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent::template Unsubscribe<TScAgent>(ctx, ScAddr::Empty, subscriptionAddrs...);
}

/*!
 * @brief Unsubscribes agent class from sc-event of adding output arc from `action_initiated` to some formed
 * sc-action.
 *
 * @tparam TScAgent An agent class to be subscribed to the event. Specified agent class must be derivied from class
 * ScAgentAction.
 * @param ctx A sc-memory context.
 */
template <class TScAgent>
typename std::enable_if<std::is_base_of<ScActionAgent, TScAgent>::value>::type UnsubscribeAgent(ScMemoryContext * ctx)
{
  ScActionAgent::template Unsubscribe<TScAgent>(ctx, ScAddr::Empty);
}

template <class TScAgent>
void BuildAndSubscribeAgent(ScMemoryContext * ctx, ScAddr const & agentImplementationAddr)
{
  ScAgentBuilder<TScAgent> builder{agentImplementationAddr};
  builder.LoadSpecification(ctx);

  TScAgent::template Subscribe<TScAgent>(ctx, agentImplementationAddr);
}

template <class TScAgent>
void DestroyAndUnsubscribeAgent(ScMemoryContext * ctx, ScAddr const & agentImplementationAddr)
{
  TScAgent::template Unsubscribe<TScAgent>(ctx, agentImplementationAddr);
}
