/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

#include "sc-memory/sc_link.hpp"

class ScMemoryGenerateElementsJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(ScAgentContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload &)
      override
  {
    ScMemoryJsonPayload responsePayload;

    auto const & resolveAddr = [&responsePayload](ScMemoryJsonPayload const & json) -> ScAddr
    {
      ScMemoryJsonPayload const & sub = json["value"];
      if (json["type"].get<std::string>() == "ref")
        return ScAddr(responsePayload[sub.get<size_t>()].get<size_t>());

      return ScAddr(sub.get<size_t>());
    };

    for (auto & atom : requestPayload)
    {
      std::string const & element = atom["el"].get<std::string>();
      ScType const & type = ScType(atom["type"].get<size_t>());

      ScAddr generatedElementAddr;
      if (element == "node")
        generatedElementAddr = context->GenerateNode(type);
      else if (element == "edge")
      {
        ScAddr const & sourceAddr = resolveAddr(atom["src"]);
        ScAddr const & targetAddr = resolveAddr(atom["trg"]);

        generatedElementAddr = context->GenerateConnector(type, sourceAddr, targetAddr);
      }
      else if (element == "link")
      {
        generatedElementAddr = context->GenerateLink(type);
        ScLink link{*context, generatedElementAddr};

        auto const & content = atom["content"];
        if (content.is_string())
          link.Set(content.get<std::string>());
        else if (content.is_number_integer())
          link.Set(content.get<sc_int>());
        else if (content.is_number_float())
          link.Set(content.get<float>());
      }

      responsePayload.push_back(generatedElementAddr.Hash());
    }

    if (responsePayload.is_null())
      return "{}"_json;

    return responsePayload;
  }
};
