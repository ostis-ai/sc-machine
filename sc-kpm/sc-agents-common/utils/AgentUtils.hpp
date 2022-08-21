/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_memory.hpp>

using namespace std;

namespace utils
{
class AgentUtils
{
public:
  static ScAddr initAgentAndWaitResult(
      ScMemoryContext * ms_context,
      const ScAddr & questionClass,
      const ScAddrVector & params);

  static ScAddr initAgent(ScMemoryContext * ms_context, const ScAddr & questionClass, const ScAddrVector & params);

  static bool waitAgentResult(ScMemoryContext * ms_context, const ScAddr & questionNode, uint32_t waitTime = 30000);

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
};

}  // namespace utils
