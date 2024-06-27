/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_wait.hpp"
#include "sc_agent.hpp"

ScWaitActionFinished::ScWaitActionFinished(ScMemoryContext const & ctx, ScAddr const & actionAddr)
  : ScWaitEvent<ScEventAddInputEdge>(ctx, actionAddr)
{
}

sc_result ScWaitActionFinished::OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
{
  return otherAddr == ScKeynodes::kQuestionFinished ? SC_RESULT_OK : SC_RESULT_NO;
}
