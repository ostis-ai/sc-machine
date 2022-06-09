#pragma once

#include "sc-memory/sc_memory.hpp"

class ScJSAction
{
public:
  virtual ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) = 0;

  virtual ~ScJSAction() = default;
};
