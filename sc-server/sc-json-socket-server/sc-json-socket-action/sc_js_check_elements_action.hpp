#pragma once

#include "sc_js_action.hpp"

class ScJSCheckElementsAction : public ScJSAction
{
public:
  ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) override
  {
    ScJSPayload responsePayload;

    for (auto & hash : requestPayload)
    {
      ScAddr const addr{hash.get<size_t>()};

      size_t type = 0;
      if (addr.IsValid())
        type = context->GetElementType(addr);

      responsePayload.push_back(type);
    }

    return responsePayload;
  }
};
