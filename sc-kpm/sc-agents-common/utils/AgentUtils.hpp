/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

using namespace std;

namespace utils
{
class AgentUtils
{
public:
  static bool applyAction(
      ScMemoryContext * ms_context,
      const ScAddr & actionClass,
      const ScAddrVector & params,
      const sc_uint32 & waitTime = DEFAULT_WAIT_TIME);

  static bool applyAction(
      ScMemoryContext * ms_context,
      const ScAddr & actionNode,
      const sc_uint32 & waitTime = DEFAULT_WAIT_TIME);

  SC_DEPRECATED(
      0.7.0,
      "Use getActionResultIfExists"
      "(ScMemoryContext * ms_context, const ScAddr & actionClass,"
      "const ScAddrVector & params,  const sc_uint32 & waitTime)"
      " instead of.")
  static ScAddr initAgentAndWaitResult(
      ScMemoryContext * ms_context,
      const ScAddr & actionClass,
      const ScAddrVector & params,
      const sc_uint32 & waitTime = DEFAULT_WAIT_TIME);

  static ScAddr getActionResultIfExists(
      ScMemoryContext * ms_context,
      const ScAddr & actionClass,
      const ScAddrVector & params,
      const sc_uint32 & waitTime = DEFAULT_WAIT_TIME);

  static ScAddr getActionResultIfExists(
      ScMemoryContext * ms_context,
      const ScAddr & actionNode,
      const sc_uint32 & waitTime = DEFAULT_WAIT_TIME);

  static ScAddr formActionNode(ScMemoryContext * ms_context, const ScAddr & actionClass, const ScAddrVector & params);

  static ScAddr initAction(ScMemoryContext * ms_context, const ScAddr & questionClass, const ScAddrVector & params);

  SC_DEPRECATED(
      0.7.0,
      "Use initAction"
      "(ScMemoryContext * ms_context, const ScAddr & questionClass, const ScAddrVector & params)"
      " instead of.")
  static ScAddr initAgent(ScMemoryContext * ms_context, const ScAddr & questionClass, const ScAddrVector & params);

  SC_DEPRECATED(
      0.7.0,
      "Use applyAction"
      "(ScMemoryContext * ms_context, const ScAddr & questionClass, "
      "const ScAddrVector & params, const sc_uint32 & waitTime)"
      " to initiate and wait for action instead of.")
  static bool waitAgentResult(ScMemoryContext * ms_context, const ScAddr & questionNode, sc_uint32 waitTime = 30000);

  static ScAddr createQuestionNode(ScMemoryContext * ms_context);

  static void assignParamsToQuestionNode(
      ScMemoryContext * ms_context,
      const ScAddr & questionNode,
      const ScAddrVector & params);

  SC_DEPRECATED(
      0.6.0,
      "Use AgentUtils::finishAgentWork"
      "(ScMemoryContext * ms_context, const ScAddr & questionNode, "
      "const ScAddrVector & answerElements, bool isSuccess = true) instead of.")
  static void finishAgentWork(
      ScMemoryContext * ms_context,
      const ScAddr & questionNode,
      const ScAddr & answer,
      bool isSuccess = true);

  static void finishAgentWork(
      ScMemoryContext * ms_context,
      const ScAddr & questionNode,
      const ScAddrVector & answerElements,
      bool isSuccess = true);

  static void finishAgentWork(ScMemoryContext * ms_context, const ScAddr & questionNode, bool isSuccess = true);

private:
  static sc_uint32 const DEFAULT_WAIT_TIME;
};

}  // namespace utils
