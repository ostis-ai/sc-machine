/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <string>
#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

using namespace std;

namespace utils
{

class AgentUtils
{
public:
  static ScAddr initAgentAndWaitResult(
        ScMemoryContext * ms_context,
        const ScAddr & questionName,
        const vector<ScAddr> & params);

  static ScAddr initAgent(
        ScMemoryContext * ms_context,
        const ScAddr & questionName,
        const vector<ScAddr> & params);

  static bool waitAgentResult(ScMemoryContext * ms_context, const ScAddr & questionNode);

  static ScAddr createQuestionNode(
        ScMemoryContext * ms_context);

  static void assignParamsToQuestionNode(
        ScMemoryContext * ms_context,
        const ScAddr & questionNode,
        const vector<ScAddr> & params);

  static void finishAgentWork(
        ScMemoryContext * ms_context,
        const ScAddr & questionNode,
        const ScAddr & answer,
        bool isSuccess = true);

static void finishAgentWork(
      ScMemoryContext * ms_context,
      const ScAddr & questionNode,
      bool isSuccess = true);

};
}
