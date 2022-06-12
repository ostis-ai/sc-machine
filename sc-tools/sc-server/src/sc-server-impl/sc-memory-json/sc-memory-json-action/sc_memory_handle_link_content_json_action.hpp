#pragma once

#include "sc_memory_json_action.hpp"

#include "sc-memory/sc_link.hpp"

class ScMemoryHandleLinkContentJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(ScMemoryContext * context, ScMemoryJsonPayload requestPayload) override
  {
    ScMemoryJsonPayload responsePayload;

    for (auto & atom : requestPayload)
    {
      std::string const & type = atom["command"];

      if (type == "set")
      {
        ScAddr const & linkAddr = ScAddr(atom["addr"].get<size_t>());
        std::string const & contentType = atom["type"];
        std::string const & data = atom["data"];

        ScLink link{*context, linkAddr};
        responsePayload.push_back(link.Set(data));
      }
      else if (type == "get")
      {
        ScAddr const & linkAddr = ScAddr(atom["addr"].get<size_t>());
        ScLink link{*context, linkAddr};

        responsePayload.push_back({{"value", link.Get<std::string>()}, {"type", "string"}});
      }
      else if (type == "find")
      {
        std::string const & data = atom["data"];

        ScAddrVector const & vector = context->FindLinksByContent(data);
        std::vector<size_t> hashes;
        for (auto const & addr : vector)
          hashes.push_back(addr.Hash());

        responsePayload.push_back(hashes);
      }
    }

    return responsePayload;
  }
};
