#pragma once

#include "sc_js_action.hpp"

#include "sc-memory/sc_link.hpp"

class ScJSCreateElementsAction : public ScJSAction
{
public:
  ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) override
  {
    ScJSPayload responsePayload;

    auto const & resolveAddr = [&responsePayload](ScJSPayload const & json) -> ScAddr {
      ScJSPayload const & sub = json["value"];
      if (json["type"] == "ref")
        return ScAddr(responsePayload[sub.get<size_t>()].get<size_t>());

      return ScAddr(sub.get<size_t>());
    };

    for (auto & atom : requestPayload)
    {
      std::string const & element = atom["el"];
      ScType const & type = ScType(atom["type"].get<size_t>());

      ScAddr created;
      if (element == "node")
        created = context->CreateNode(type);
      else if (element == "edge")
      {
        ScAddr const & src = resolveAddr(atom["src"]);
        ScAddr const & trg = resolveAddr(atom["trg"]);

        created = context->CreateEdge(type, src, trg);
      }
      else if (element == "link")
      {
        created = context->CreateLink(type);
        ScLink link{*context, created};
        link.Set(atom["content"].get<std::string>());
      }

      responsePayload.push_back(created.Hash());
    }

    if (responsePayload.is_null())
      return "{}"_json;

    return responsePayload;
  }
};
