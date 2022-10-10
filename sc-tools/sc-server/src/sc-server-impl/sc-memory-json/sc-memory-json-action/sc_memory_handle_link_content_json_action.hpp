/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

#include "sc-memory/sc_link.hpp"

#include "sc-memory/utils/sc_base64.hpp"

class ScMemoryHandleLinkContentJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScMemoryContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload & errorsPayload) override
  {
    ScMemoryJsonPayload responsePayload;

    auto const & process = [&context, &responsePayload, this](ScMemoryJsonPayload const & atom)
    {
      std::string const & type = atom["command"].get<std::string>();

      if (type == "set")
        responsePayload.push_back(SetContent(context, atom));
      else if (type == "get")
        responsePayload.push_back(GetContent(context, atom));
      else if (type == "find")
        responsePayload.push_back(FindLinksByContent(context, atom));
      else if (type == "find_by_substr" || type == "find_links_by_substr")
        responsePayload.push_back(FindLinksByContentSubstring(context, atom));
      else if (type == "find_strings_by_substr")
        responsePayload.push_back(FindLinksContentsByContentSubstring(context, atom));
    };

    if (requestPayload.is_array())
    {
      for (auto & atom : requestPayload)
        process(atom);
    }
    else
      process(requestPayload);

    return responsePayload;
  }

private:
  sc_bool SetContent(ScMemoryContext * context, ScMemoryJsonPayload const & atom)
  {
    ScAddr const & linkAddr = ScAddr(atom["addr"].get<size_t>());
    std::string const & contentType = atom["type"].get<std::string>();
    auto const & data = atom["data"];

    ScLink link{*context, linkAddr};

    if (contentType == "string" || contentType == "binary")
      return link.Set(data.get<std::string>());
    else if (contentType == "int")
      return link.Set(data.get<sc_int>());
    else if (contentType == "float")
      return link.Set(data.get<float>());

    return SC_FALSE;
  }

  ScMemoryJsonPayload GetContent(ScMemoryContext * context, ScMemoryJsonPayload const & atom)
  {
    ScAddr const & linkAddr = ScAddr(atom["addr"].get<size_t>());
    ScLink link{*context, linkAddr};

    ScMemoryJsonPayload answer;
    if (link.DetermineType() >= ScLink::Type::Int8 && link.DetermineType() <= ScLink::Type::UInt64)
      return {{"value", link.Get<sc_int>()}, {"type", "int"}};
    else if (link.IsType<double>() || link.IsType<float>())
      return {{"value", link.Get<float>()}, {"type", "float"}};
    else
      return {{"value", link.Get<std::string>()}, {"type", "string"}};
  }

  std::vector<size_t> FindLinksByContent(ScMemoryContext * context, ScMemoryJsonPayload const & atom)
  {
    auto const & data = atom["data"];
    ScAddrVector vector;
    if (data.is_string())
      vector = context->FindLinksByContent(data.get<std::string>());
    else if (data.is_number_integer())
      vector = context->FindLinksByContent(std::to_string(data.get<sc_int>()));
    else if (data.is_number_float())
    {
      std::stringstream stream;
      stream << data.get<float>();
      vector = context->FindLinksByContent(stream.str());
    }

    std::vector<size_t> hashes;
    for (auto const & addr : vector)
      hashes.push_back(addr.Hash());

    return hashes;
  }

  std::vector<size_t> FindLinksByContentSubstring(ScMemoryContext * context, ScMemoryJsonPayload const & atom)
  {
    auto const & data = atom["data"];
    ScAddrVector vector;
    if (data.is_string())
      vector = context->FindLinksByContentSubstring(data.get<std::string>(), maxLengthToSearchAsPrefix);
    else if (data.is_number_integer())
      vector = context->FindLinksByContentSubstring(std::to_string(data.get<sc_int>()), maxLengthToSearchAsPrefix);
    else if (data.is_number_float())
    {
      std::stringstream stream;
      stream << data.get<float>();
      vector = context->FindLinksByContentSubstring(stream.str(), maxLengthToSearchAsPrefix);
    }

    std::vector<size_t> hashes;
    for (auto const & addr : vector)
      hashes.push_back(addr.Hash());

    return hashes;
  }

  std::vector<std::string> FindLinksContentsByContentSubstring(ScMemoryContext * context, ScMemoryJsonPayload const & atom)
  {
    auto const & data = atom["data"];
    std::vector<std::string> vector;
    if (data.is_string())
      vector = context->FindLinksContentsByContentSubstring(data.get<std::string>(), maxLengthToSearchAsPrefix);
    else if (data.is_number_integer())
      vector = context->FindLinksContentsByContentSubstring(std::to_string(data.get<sc_int>()), maxLengthToSearchAsPrefix);
    else if (data.is_number_float())
    {
      std::stringstream stream;
      stream << data.get<float>();
      vector = context->FindLinksContentsByContentSubstring(stream.str(), maxLengthToSearchAsPrefix);
    }

    return vector;
  }

private:
  sc_uint32 const maxLengthToSearchAsPrefix = 4;
};
