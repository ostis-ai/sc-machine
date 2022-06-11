#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryHandleKeynodesJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(ScMemoryContext * context, ScMemoryJsonPayload requestPayload) override
  {
    ScMemoryJsonPayload responsePayload;

    for (auto & atom : requestPayload)
    {
      std::string const & idtf = atom["idtf"];
      std::string const & type = atom["command"];

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
    return responsePayload;
  }
};
