/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

namespace utils
{
class AgentUtils
{
public:
  static bool applyAction(
      ScMemoryContext * ms_context,
      ScAddr const & actionClass,
      ScAddrVector const & params,
      sc_uint32 const & waitTime = DEFAULT_WAIT_TIME);

  static bool applyAction(
      ScMemoryContext * ms_context,
      ScAddr const & actionNode,
      sc_uint32 const & waitTime = DEFAULT_WAIT_TIME,
      ScAddr onEventClassAddr = ScAddr::Empty);

  SC_DEPRECATED(
      0.9.0,
      "Use ScAddr applyActionAndGetResultIfExists"
      "ScMemoryContext * ms_context, const ScAddr & actionClass, const ScAddrVector & params, "
      "const sc_uint32 & waitTime) instead of. It will be removed in sc-machine 0.10.0.")
  static ScAddr getActionResultIfExists(
      ScMemoryContext * ms_context,
      ScAddr const & actionClass,
      ScAddrVector const & params,
      sc_uint32 const & waitTime = DEFAULT_WAIT_TIME);

  static ScAddr applyActionAndGetResultIfExists(
      ScMemoryContext * ms_context,
      ScAddr const & actionClass,
      ScAddrVector const & params,
      sc_uint32 const & waitTime = DEFAULT_WAIT_TIME);

  SC_DEPRECATED(
      0.9.0,
      "Use ScAddr applyActionAndGetResultIfExists"
      "ScMemoryContext * ms_context, const ScAddr & actionNode, const sc_uint32 & waitTime) instead of. "
      "It will be removed in sc-machine 0.10.0.")
  static ScAddr getActionResultIfExists(
      ScMemoryContext * ms_context,
      ScAddr const & actionNode,
      sc_uint32 const & waitTime = DEFAULT_WAIT_TIME);

  static ScAddr applyActionAndGetResultIfExists(
      ScMemoryContext * ms_context,
      ScAddr const & actionNode,
      sc_uint32 const & waitTime = DEFAULT_WAIT_TIME);

  static ScAddr formActionNode(ScMemoryContext * ms_context, ScAddr const & actionClass, ScAddrVector const & params);

  static ScAddr initAction(ScMemoryContext * ms_context, ScAddr const & actionClass, ScAddrVector const & params);

  static ScAddr createQuestionNode(ScMemoryContext * ms_context);

  static void assignParamsToQuestionNode(
      ScMemoryContext * ms_context,
      ScAddr const & actionNode,
      ScAddrVector const & params);

  static void finishAgentWork(
      ScMemoryContext * ms_context,
      ScAddr const & actionNode,
      ScAddrVector const & answerElements,
      bool isSuccess = true);

  static void finishAgentWork(ScMemoryContext * ms_context, ScAddr const & actionNode, bool isSuccess = true);

private:
  static sc_uint32 const DEFAULT_WAIT_TIME;
};

}  // namespace utils
