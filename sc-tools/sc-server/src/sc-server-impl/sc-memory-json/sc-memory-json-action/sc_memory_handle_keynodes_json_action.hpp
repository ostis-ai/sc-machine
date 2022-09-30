/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryHandleKeynodesJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScMemoryContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload & statusPayload) override
  {
    ScMemoryJsonPayload responsePayload;

    for (auto & atom : requestPayload)
    {
      std::string const & idtf = atom["idtf"].get<std::string>();
      std::string const & type = atom["command"].get<std::string>();

      ScAddr keynode;
      if (type == "find")
        keynode = context->HelperFindBySystemIdtf(idtf);
      else if (type == "resolve")
      {
        ScType const & elType = ScType(atom["elType"].get<size_t>());
        keynode = context->HelperResolveSystemIdtf(idtf, elType);
      }

      responsePayload.push_back(keynode.Hash());
    }

    if (responsePayload.is_null())
      return "{}"_json;

    return responsePayload;
  }
};
