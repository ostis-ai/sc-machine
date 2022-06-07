#pragma once

#include "sc_js_action.hpp"

#include "sc-memory/sc_link.hpp"

class ScJSHandleContentAction : public ScJSAction
{
public:
  ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) override
  {
    ScJSPayload responsePayload;

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

        ScStreamPtr stream;
        auto * sc_string = new sc_char[data.size()];
        std::strcpy(sc_string, data.c_str());
        size_t written = 0;

        stream->Write(sc_string, data.size(), written);
        ScAddrVector const & vector = context->FindLinksByContent(stream);
        std::vector<size_t> hashes;
        for (auto const & addr : vector)
          hashes.push_back(addr.Hash());

        responsePayload.push_back(hashes);
      }
    }

    return responsePayload;
  }
};
