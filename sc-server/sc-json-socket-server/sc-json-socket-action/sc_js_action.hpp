#pragma once

#include "sc-memory/sc_memory.hpp"

#include "../sc_js_payload.hpp"


class ScJSAction
{
public:
  virtual ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) = 0;
};
