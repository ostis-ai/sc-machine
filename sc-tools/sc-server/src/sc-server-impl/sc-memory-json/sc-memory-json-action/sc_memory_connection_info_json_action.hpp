/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryConnectionInfoJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScMemoryContext * context,
      ScMemoryJsonPayload requestPayload,
      ScMemoryJsonPayload & errorsPayload) override
  {
    ScMemoryJsonPayload responsePayload{
        {"connection_id", (sc_uint64)context}, {"user_addr", context->GetUserAddr().Hash()}};

    return responsePayload;
  }
};
