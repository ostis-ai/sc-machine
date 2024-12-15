/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_handle_link_content_json_action.hpp"

#include <sc-memory/sc_link.hpp>
#include <sc-memory/sc_agent_context.hpp>

#include <sc-memory/utils/sc_base64.hpp>

ScMemoryJsonPayload ScMemoryHandleLinkContentJsonAction::Complete(
    ScAgentContext * context,
    ScMemoryJsonPayload requestPayload,
    ScMemoryJsonPayload & errorsPayload)
{
  ScMemoryJsonPayload responsePayload;

  auto const & process = [&context, &responsePayload, this](ScMemoryJsonPayload const & parameters)
  {
    ScMemoryJsonPayload error;
    if (!parameters.contains("command"))
    {
      error = {
          "Not able to handle command for sc-links because type of command to be completed is not specified. Command "
          "`content` must have payload with `command` parameter."};
      return error;
    }

    if (!parameters["command"].is_string())
    {
      error = {
          "Not able to handle command for sc-links because type of command to be completed is specified incorrectly. "
          "Command `set` must have payload with `command` parameter having string value."};
      return error;
    }

    std::string const & commandType = parameters["command"].get<std::string>();
    ScMemoryJsonPayload result;
    if (commandType == "set")
      result = SetLinkContent(context, parameters, error);
    else if (commandType == "get")
      result = GetLinkContent(context, parameters, error);
    else if (commandType == "find")
      result = SearchLinksByContent(context, parameters, error);
    else if (commandType == "find_by_substr" || commandType == "find_links_by_substr")
      result = SearchLinksByContentSubstring(context, parameters, error);
    else if (commandType == "find_strings_by_substr")
      result = SearchLinksContentsByContentSubstring(context, parameters, error);
    else
      error = {
          "Not able to handle command for sc-links because type of command to be completed is specified incorrectly. "
          "Command `set` must have payload with `command` parameter having one of string values: `set`, `get`, "
          "`find`, `find_links_by_substr` or `find_strings_by_substr`."};

    if (error.is_null())
      responsePayload.push_back(result);

    return error;
  };

  if (requestPayload.is_array())
  {
    for (size_t i = 0; i < requestPayload.size(); ++i)
    {
      ScMemoryJsonPayload const & error = process(requestPayload[i]);
      if (!error.is_null())
        errorsPayload.push_back({i, error});
    }
  }
  else
    errorsPayload = process(requestPayload);

  return responsePayload;
}

ScMemoryJsonPayload ScMemoryHandleLinkContentJsonAction::SetLinkContent(
    ScAgentContext * context,
    ScMemoryJsonPayload const & parameters,
    ScMemoryJsonPayload & error)
{
  bool result = false;
  if (!parameters.contains("addr"))
  {
    error = {
        "Not able to set content for sc-link because its sc-address hash is not specified in command. Payload with "
        "`set` command must have `addr` parameter."};
    return result;
  }

  if (!parameters["addr"].is_number_integer())
  {
    error = {
        "Not able to set content for sc-link because its sc-address hash is specified incorrectly. Payload with "
        "`set` command must have `addr` parameter with integer value."};
    return result;
  }

  ScAddr linkAddr{parameters["addr"].get<size_t>()};
  ScLink link{*context, linkAddr};

  if (!parameters.contains("type"))
  {
    error = {
        "Not able to set content for sc-link because type of content to be set is not specified. Payload with `set` "
        "command must have `type` parameter."};
    return result;
  }

  if (!parameters["type"].is_string())
  {
    error = {
        "Not able to set content for sc-link because type of content to be set is specified incorrectly. Payload "
        "with `set` command must have `type` parameter with string value."};
    return result;
  }

  std::string const & contentType = parameters["type"].get<std::string>();

  if (!parameters.contains("data"))
  {
    error = {
        "Not able to set content for sc-link because content to be set is not specified. Payload with `set` command "
        "must have `data` parameter."};
    return result;
  }

  auto const & data = parameters["data"];
  if (contentType == "string" || contentType == "binary")
  {
    if (!data.is_string())
    {
      error = {
          "Not able to set content for sc-link because content to be set is specified incorrectly. Payload with "
          "`set` command has `data` parameter with not string value, but `type` parameter is specified as `string`."};
      return result;
    }

    result = link.Set(data.get<std::string>());
  }
  else if (contentType == "int")
  {
    if (!data.is_number_integer())
    {
      error = {
          "Not able to set content for sc-link because content to be set is specified incorrectly. Payload with "
          "`set` command has `data` parameter with not integer value, but `type` parameter is specified as `int`."};
      return result;
    }

    result = link.Set(data.get<sc_int>());
  }
  else if (contentType == "float")
  {
    if (!data.is_number_float())
    {
      error = {
          "Not able to set content for sc-link because content to be set is specified incorrectly. Payload with "
          "`set` command has `data` parameter with not float value, but `type` parameter is specified as `float`."};
      return result;
    }

    result = link.Set(data.get<sc_float>());
  }
  else
  {
    error = {
        "Not able to set content for sc-link because type of content to be set is specified incorrectly. Payload "
        "with `set` command must have `type` parameter with one of the following values: `string`, `int` or `float`"};
  }

  return result;
}

ScMemoryJsonPayload ScMemoryHandleLinkContentJsonAction::GetLinkContent(
    ScAgentContext * context,
    ScMemoryJsonPayload const & parameters,
    ScMemoryJsonPayload & error)
{
  ScMemoryJsonPayload result;
  if (!parameters.contains("addr"))
  {
    error = {
        "Not able to get content of sc-link because its sc-address hash is not specified in command. Payload with "
        "`get` command must have `addr` parameter."};
    return result;
  }

  if (!parameters["addr"].is_number_integer())
  {
    error = {
        "Not able to get content of sc-link because its sc-address hash is specified incorrectly. Payload with "
        "`get` command must have `addr` parameter with integer value."};
    return result;
  }

  ScAddr linkAddr{parameters["addr"].get<size_t>()};
  ScLink link{*context, linkAddr};

  if (link.DetermineType() >= ScLink::Type::Int8 && link.DetermineType() <= ScLink::Type::UInt64)
    result = {{"value", link.Get<sc_int>()}, {"type", "int"}};
  else if (link.IsType<double>() || link.IsType<float>())
    result = {{"value", link.Get<float>()}, {"type", "float"}};
  else
    result = {{"value", link.Get<std::string>()}, {"type", "string"}};
  return result;
}

ScMemoryJsonPayload ScMemoryHandleLinkContentJsonAction::SearchLinksByContent(
    ScAgentContext * context,
    ScMemoryJsonPayload const & parameters,
    ScMemoryJsonPayload & error)
{
  std::set<size_t> hashes;
  if (!parameters.contains("data"))
  {
    error = {
        "Not able to search sc-links by content because content to be search is not specified. Payload with `find` "
        "command must have `data` parameter."};
    return hashes;
  }

  ScAddrSet linkSet;
  auto const & data = parameters["data"];
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
  else
  {
    error = {
        "Not able to search sc-links by content because content to be search is specified incorrectly. Payload with "
        "`find` command must have `data` parameter with `string`, `integer` or `float` value."};
  }

  for (auto const & linkAddr : linkSet)
    hashes.insert(linkAddr.Hash());

  return hashes;
}

ScMemoryJsonPayload ScMemoryHandleLinkContentJsonAction::SearchLinksByContentSubstring(
    ScAgentContext * context,
    ScMemoryJsonPayload const & parameters,
    ScMemoryJsonPayload & error)
{
  std::set<size_t> hashes;
  if (!parameters.contains("data"))
  {
    error = {
        "Not able to search sc-links by content substring because content substring to be search is not specified. "
        "Payload with `find_links_by_substr` command must have `data` parameter."};
    return hashes;
  }

  ScAddrSet linkSet;
  auto const & data = parameters["data"];
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
  else
  {
    error = {
        "Not able to search sc-links by content substring because content substring to be searched is specified "
        "incorrectly. Payload with `find_links_by_substr` command must have `data` parameter with `string`, `integer` "
        "or `float` value."};
  }

  for (auto const & linkAddr : linkSet)
    hashes.insert(linkAddr.Hash());

  return hashes;
}

ScMemoryJsonPayload ScMemoryHandleLinkContentJsonAction::SearchLinksContentsByContentSubstring(
    ScAgentContext * context,
    ScMemoryJsonPayload const & parameters,
    ScMemoryJsonPayload & error)
{
  if (!parameters.contains("data"))
  {
    error = {
        "Not able to search contents of sc-links by content substring because content substring to be searched is "
        "not specified. Payload with `find_strings_by_substr` command must have `data` parameter."};
    return {};
  }

  auto const & data = parameters["data"];
  std::set<std::string> linkContentSet;
  if (data.is_string())
    linkContentSet = context->SearchLinksContentsByContentSubstring(data.get<std::string>(), maxLengthToSearchAsPrefix);
  else if (data.is_number_integer())
    linkContentSet =
        context->SearchLinksContentsByContentSubstring(std::to_string(data.get<sc_int>()), maxLengthToSearchAsPrefix);
  else if (data.is_number_float())
  {
    std::stringstream stream;
    stream << data.get<float>();
    linkContentSet = context->SearchLinksContentsByContentSubstring(stream.str(), maxLengthToSearchAsPrefix);
  }
  else
  {
    error = {
        "Not able to search contents of sc-links by content substring because content substring to be searched is "
        "specified incorrectly. Payload with `find_strings_by_substr` command must have `data` parameter with "
        "`string`, `integer` or `float` value."};
  }

  return linkContentSet;
}
