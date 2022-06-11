#pragma once

#include "sc-memory/sc_memory.hpp"
#include "../sc_memory_json_payload.hpp"

class ScMemoryJsonAction
{
public:
  virtual ScMemoryJsonPayload Complete(ScMemoryContext * context, ScMemoryJsonPayload requestPayload) = 0;

  virtual ~ScMemoryJsonAction() = default;
};
