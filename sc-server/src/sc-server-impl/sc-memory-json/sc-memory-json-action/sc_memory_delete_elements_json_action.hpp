#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryDeleteElementsJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(ScMemoryContext * context, ScMemoryJsonPayload requestPayload) override
  {
    for (auto & hash : requestPayload)
    {
      ScAddr const addr{hash.get<size_t>()};
      context->EraseElement(addr);
    }

    return {SC_TRUE};
  }
};
