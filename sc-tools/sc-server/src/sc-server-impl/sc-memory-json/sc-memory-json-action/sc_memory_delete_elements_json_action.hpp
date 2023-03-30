/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryDeleteElementsJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScMemoryContext * context,
      ScMemoryJsonPayload requestPayload,
      ScMemoryJsonPayload & errorsPayload) override
  {
    ScAddr const & questionEraseElements = context->HelperResolveSystemIdtf("question_erase_elements", ScType::NodeConst);
    ScAddr const & rrelAddr = context->HelperResolveSystemIdtf("rrel_1");
    ScAddr const & questionInitiated = context->HelperResolveSystemIdtf("question_initiated", ScType::NodeConst);

    ScAddr const & questionAddr = context->CreateNode(ScType::NodeConst);
    ScAddr const & toEraseElements = context->CreateNode(ScType::NodeConst);

    ScAddr const & questionEdge = context->CreateEdge(ScType::EdgeAccessConstPosPerm, questionAddr, toEraseElements);
    context->CreateEdge(ScType::EdgeAccessConstPosPerm, rrelAddr, questionEdge);
    context->CreateEdge(ScType::EdgeAccessConstPosPerm, questionEraseElements, questionAddr);

    for (auto & hash : requestPayload)
    {
      ScAddr const addr{hash.get<size_t>()};
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, toEraseElements, addr);
    }

    context->CreateEdge(ScType::EdgeAccessConstPosPerm, questionInitiated, questionAddr);

    return {SC_TRUE};
  }
};
