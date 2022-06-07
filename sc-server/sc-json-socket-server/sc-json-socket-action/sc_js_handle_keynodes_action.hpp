#pragma once

#include "sc_js_action.hpp"


class ScJSHandleKeynodesAction : public ScJSAction
{
public:
  ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) override
  {
    ScJSPayload responsePayload;

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
