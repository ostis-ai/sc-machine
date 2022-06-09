#pragma once

#include "sc_js_action.hpp"

class ScJSDeleteElementsAction : public ScJSAction
{
public:
  ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) override
  {
    for (auto & hash : requestPayload)
    {
      ScAddr const addr{hash.get<size_t>()};
      context->EraseElement(addr);
    }

    return {SC_TRUE};
  }
};
