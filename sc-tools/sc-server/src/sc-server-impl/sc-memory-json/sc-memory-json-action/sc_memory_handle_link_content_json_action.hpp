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
  ScMemoryJsonPayload Complete(ScAgentContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload &)
      override
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
        responsePayload.push_back(SearchLinksByContent(context, atom));
      else if (type == "find_by_substr" || type == "find_links_by_substr")
        responsePayload.push_back(SearchLinksByContentSubstring(context, atom));
      else if (type == "find_strings_by_substr")
        responsePayload.push_back(SearchLinksContentsByContentSubstring(context, atom));
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
  sc_bool SetContent(ScAgentContext * context, ScMemoryJsonPayload const & atom)
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

  ScMemoryJsonPayload GetContent(ScAgentContext * context, ScMemoryJsonPayload const & atom)
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

  std::vector<size_t> SearchLinksByContent(ScAgentContext * context, ScMemoryJsonPayload const & atom)
  {
    auto const & data = atom["data"];
    ScAddrSet linkSet;
    if (data.is_string())
      linkSet = context->SearchLinksByContent(data.get<std::string>());
    else if (data.is_number_integer())
      linkSet = context->SearchLinksByContent(std::to_string(data.get<sc_int>()));
    else if (data.is_number_float())
    {
      std::stringstream stream;
      stream << data.get<float>();
      linkSet = context->SearchLinksByContent(stream.str());
    }

    std::vector<size_t> hashes;
    hashes.reserve(linkSet.size());
    for (auto const & linkAddr : linkSet)
      hashes.push_back(linkAddr.Hash());

    return hashes;
  }

  std::vector<size_t> SearchLinksByContentSubstring(ScAgentContext * context, ScMemoryJsonPayload const & atom)
  {
    auto const & data = atom["data"];
    ScAddrSet linkSet;
    if (data.is_string())
      linkSet = context->SearchLinksByContentSubstring(data.get<std::string>(), maxLengthToSearchAsPrefix);
    else if (data.is_number_integer())
      linkSet = context->SearchLinksByContentSubstring(std::to_string(data.get<sc_int>()), maxLengthToSearchAsPrefix);
    else if (data.is_number_float())
    {
      std::stringstream stream;
      stream << data.get<float>();
      linkSet = context->SearchLinksByContentSubstring(stream.str(), maxLengthToSearchAsPrefix);
    }

    std::vector<size_t> hashes;
    hashes.reserve(linkSet.size());
    for (auto const & linkAddr : linkSet)
      hashes.push_back(linkAddr.Hash());

    return hashes;
  }

  std::vector<std::string> SearchLinksContentsByContentSubstring(
      ScAgentContext * context,
      ScMemoryJsonPayload const & atom)
  {
    auto const & data = atom["data"];
    std::set<std::string> linkContentSet;
    if (data.is_string())
      linkContentSet =
          context->SearchLinksContentsByContentSubstring(data.get<std::string>(), maxLengthToSearchAsPrefix);
    else if (data.is_number_integer())
      linkContentSet =
          context->SearchLinksContentsByContentSubstring(std::to_string(data.get<sc_int>()), maxLengthToSearchAsPrefix);
    else if (data.is_number_float())
    {
      std::stringstream stream;
      stream << data.get<float>();
      linkContentSet = context->SearchLinksContentsByContentSubstring(stream.str(), maxLengthToSearchAsPrefix);
    }

    return {linkContentSet.cbegin(), linkContentSet.cend()};
  }

private:
  sc_uint32 const maxLengthToSearchAsPrefix = 4;
};
