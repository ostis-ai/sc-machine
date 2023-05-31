/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_check_elements_json_action.hpp"

ScMemoryJsonPayload ScMemoryCheckElementsJsonAction::Complete(
    ScMemoryContext * context,
    ScMemoryJsonPayload requestPayload,
    ScMemoryJsonPayload & errorsPayload)
{
  ScMemoryJsonPayload responsePayload;

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
