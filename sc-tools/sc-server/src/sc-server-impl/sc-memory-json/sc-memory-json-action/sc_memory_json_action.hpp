/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory.hpp"
#include "../sc_memory_json_payload.hpp"

class ScMemoryJsonAction
{
public:
  virtual ScMemoryJsonPayload Complete(
      ScMemoryContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload & errorsPayload) = 0;

  virtual ~ScMemoryJsonAction() = default;
};
