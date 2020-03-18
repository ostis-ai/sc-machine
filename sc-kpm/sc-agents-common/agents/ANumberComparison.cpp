/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/
#include <iostream>

#include <sc-memory/cpp/sc_memory.hpp>
#include <sc-memory/cpp/sc_stream.hpp>

#include "ANumberComparison.hpp"
#include "utils/AgentUtils.hpp"
#include "utils/CommonUtils.hpp"


using namespace std;
using namespace utils;

namespace scAgentsCommon
{

SC_AGENT_IMPLEMENTATION(ANumberComparison)
{
  if (!edgeAddr.IsValid())
    return SC_RESULT_ERROR;

  ScAddr question = ms_context->GetEdgeTarget(edgeAddr);

  if (!ms_context->HelperCheckEdge(CoreKeynodes::question, question, ScType::EdgeAccessConstPosPerm))
  {
    cout << "This is not a question\n";
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  if (!ms_context->HelperCheckEdge(CoreKeynodes::question_initiated, question, ScType::EdgeAccessConstPosPerm))
  {
    cout << "This question was not initiated\n";
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  ScIterator5Ptr iter_firstNumber = ms_context->Iterator5(
        question, ScType::EdgeAccessConstPosPerm, ScType::NodeConst,
        ScType::EdgeAccessConstPosPerm, CoreKeynodes::rrel_1);
  ScAddr firstNumber;
  ScAddr secondNumber;

  while (iter_firstNumber->Next())
  {
    firstNumber = iter_firstNumber->Get(2);

    ScIterator5Ptr iter_secondNumber = ms_context->Iterator5(
          question, ScType::EdgeAccessConstPosPerm, ScType::NodeConst,
          ScType::EdgeAccessConstPosPerm, CoreKeynodes::rrel_2);
    while (iter_secondNumber->Next())
    {
      secondNumber = iter_secondNumber->Get(2);
    }

    if (!ms_context->HelperCheckEdge(CoreKeynodes::number, firstNumber, ScType::EdgeAccessConstPosPerm) ||
        !ms_context->HelperCheckEdge(CoreKeynodes::number, secondNumber, ScType::EdgeAccessConstPosPerm))
    {
      cout << "Parameters provided aren't numbers\n";
      return SC_RESULT_ERROR_INVALID_PARAMS;
    }
  }

  ScIterator5Ptr iter_firstNumberLink = ms_context->Iterator5(
        firstNumber, ScType::EdgeDCommonConst, ScType::Unknown,
        ScType::EdgeAccessConstPosPerm, CoreKeynodes::nrel_idtf);
  ScIterator5Ptr iter_secondNumberLink = ms_context->Iterator5(
        secondNumber, ScType::EdgeDCommonConst, ScType::Unknown,
        ScType::EdgeAccessConstPosPerm, CoreKeynodes::nrel_idtf);

  int delta;
  if (iter_firstNumberLink->Next() && iter_secondNumberLink->Next())
  {
    ScAddr fLink = iter_firstNumberLink->Get(2);
    ScAddr sLink = iter_secondNumberLink->Get(2);
    int firstNumberValue = stoi(CommonUtils::readString(ms_context.get(), fLink));
    int secondNumberValue = stoi(CommonUtils::readString(ms_context.get(), sLink));
    delta = firstNumberValue - secondNumberValue;
  }
  else
  {
    cout << "Some numbers have no value\n";
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  cout << "delta: " << delta << endl;

  ScAddr numsRelEdge;
  ScAddr relEdge;
  ScAddr rel;

  if (delta == 0)
  {
    numsRelEdge = ms_context->CreateEdge(ScType::ScType::EdgeUCommonConst, firstNumber, secondNumber);
    relEdge = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm,
                                     CoreKeynodes::nrel_equal_numbers, numsRelEdge);
    rel = CoreKeynodes::nrel_equal_numbers;
    cout << "Numbers are equal\n";
  }
  else
  {
    if (delta > 0)
    {
      numsRelEdge = ms_context->CreateEdge(ScType::ScType::EdgeDCommonConst,
                                           firstNumber, secondNumber);
      cout << "First number is more than second number\n";
    }
    else
    {
      numsRelEdge = ms_context->CreateEdge(ScType::ScType::EdgeDCommonConst,
                                           secondNumber, firstNumber);
      cout << "First number is less than second number\n";
    }

    relEdge = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm,
                                     CoreKeynodes::nrel_be_more, numsRelEdge);
    rel = CoreKeynodes::nrel_be_more;
  }

  ScAddr answer = ms_context->CreateNode(ScType::NodeConstStruct);

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm,
                         answer, firstNumber);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm,
                         answer, secondNumber);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm,
                         answer, numsRelEdge);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm,
                         answer, relEdge);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm,
                         answer, rel);

  utils::AgentUtils::finishAgentWork((ScMemoryContext *) ms_context.get(), question, answer);

  return SC_RESULT_OK;
}

} // namespace scAgentsCommon
