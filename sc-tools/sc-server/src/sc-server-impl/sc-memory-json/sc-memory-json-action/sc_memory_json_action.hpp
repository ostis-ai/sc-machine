/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-server-impl/sc-memory-json/sc_memory_json_payload.hpp"

class ScAgentContext;

class ScMemoryJsonAction
{
public:
  virtual ScMemoryJsonPayload Complete(
      ScAgentContext * context,
      ScMemoryJsonPayload requestPayload,
      ScMemoryJsonPayload & errorsPayload) = 0;

  virtual ~ScMemoryJsonAction() = default;
};
