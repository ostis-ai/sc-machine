/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */


#pragma once

#include "sc_memory_json_action.hpp"

#include "sc-memory/sc_link.hpp"

class ScMemoryCreateElementsJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScMemoryContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload & errorsPayload) override
  {
    ScMemoryJsonPayload responsePayload;

    auto const & resolveAddr = [&responsePayload](ScMemoryJsonPayload const & json) -> ScAddr {
      ScMemoryJsonPayload const & sub = json["value"];
      if (json["type"].get<std::string>() == "ref")
        return ScAddr(responsePayload[sub.get<size_t>()].get<size_t>());

      return ScAddr(sub.get<size_t>());
    };

    for (auto & atom : requestPayload)
    {
      std::string const & element = atom["el"].get<std::string>();
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

        auto const & content = atom["content"];
        if (content.is_string())
          link.Set(content.get<std::string>());
        else if (content.is_number_integer())
          link.Set(content.get<sc_int>());
        else if (content.is_number_float())
          link.Set(content.get<float>());
      }

      responsePayload.push_back(created.Hash());
    }

    if (responsePayload.is_null())
      return "{}"_json;

    return responsePayload;
  }
};
