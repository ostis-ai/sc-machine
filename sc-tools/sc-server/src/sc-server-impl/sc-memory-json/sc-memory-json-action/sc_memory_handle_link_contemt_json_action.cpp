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
    if (!parameters.contains(COMMAND))
    {
      error = {
          "Not able to handle command for sc-links because type of command to be completed is not specified. Command `"
          + HANDLE_LINK_CONTENT + "` must have payload with `" + COMMAND + "` parameter."};
      return error;
    }

    if (!parameters[COMMAND].is_string())
    {
      error = {
          "Not able to handle command for sc-links because type of command to be completed is specified incorrectly. "
          "Command `"
          + HANDLE_LINK_CONTENT + "` must have payload with `" + COMMAND + "` parameter having string value."};
      return error;
    }

    std::string const & commandType = parameters[COMMAND].get<std::string>();
    ScMemoryJsonPayload result;
    if (commandType == SET_LINK_CONTENT)
      result = SetLinkContent(context, parameters, error);
    else if (commandType == GET_LINK_CONTENT)
      result = GetLinkContent(context, parameters, error);
    else if (commandType == SEARCH_LINKS_BY_CONTENT)
      result = SearchLinksByContent(context, parameters, error);
    else if (commandType == "find_by_substr" || commandType == SEARCH_LINKS_BY_CONTENT_SUBSTRING)
      result = SearchLinksByContentSubstring(context, parameters, error);
    else if (commandType == SEARCH_LINKS_CONTENTS_BY_CONTENT_SUBSTRING)
      result = SearchLinksContentsByContentSubstring(context, parameters, error);
    else
      error = {
          "Not able to handle command for sc-links because type of command to be completed is specified incorrectly. "
          "Command `" + HANDLE_LINK_CONTENT + "` must have payload with `" + COMMAND + "` parameter having one of "
          "string values: `" + SET_LINK_CONTENT + "`, `" + GET_LINK_CONTENT + "`, `" + SEARCH_LINKS_BY_CONTENT + "`, `"
          + SEARCH_LINKS_BY_CONTENT_SUBSTRING + "` or `" + SEARCH_LINKS_CONTENTS_BY_CONTENT_SUBSTRING + "`."};

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
  if (!parameters.contains(LINK_ADDRESS))
  {
    error = {
        "Not able to set content for sc-link because its sc-address hash is not specified in command. Payload with "
        "`"
        + SET_LINK_CONTENT + "` command must have `" + LINK_ADDRESS + "` parameter."};
    return result;
  }

  if (!parameters[LINK_ADDRESS].is_number_integer())
  {
    error = {
        "Not able to set content for sc-link because its sc-address hash is specified incorrectly. Payload with `"
        + SET_LINK_CONTENT + "` command must have `" + LINK_ADDRESS + "` parameter with integer value."};
    return result;
  }

  ScAddr linkAddr{parameters[LINK_ADDRESS].get<size_t>()};
  ScLink link{*context, linkAddr};

  if (!parameters.contains(LINK_CONTENT_TYPE))
  {
    error = {
        "Not able to set content for sc-link because type of content to be set is not specified. Payload with `"
        + SET_LINK_CONTENT + "` command must have `" + LINK_CONTENT_TYPE + "` parameter."};
    return result;
  }

  if (!parameters[LINK_CONTENT_TYPE].is_string())
  {
    error = {
        "Not able to set content for sc-link because type of content to be set is specified incorrectly. Payload "
        "with `" + SET_LINK_CONTENT + "` command must have `" + LINK_CONTENT_TYPE + "` parameter with string "
        "value."};
    return result;
  }

  std::string const & contentType = parameters[LINK_CONTENT_TYPE].get<std::string>();

  if (!parameters.contains(LINK_CONTENT))
  {
    error = {
        "Not able to set content for sc-link because content to be set is not specified. Payload with `"
        + SET_LINK_CONTENT + "` command must have `" + LINK_CONTENT + "` parameter."};
    return result;
  }

  auto const & data = parameters[LINK_CONTENT];
  if (contentType == STRING_CONTENT_TYPE || contentType == "binary")
  {
    if (!data.is_string())
    {
      error = {
          "Not able to set content for sc-link because content to be set is specified incorrectly. Payload with `"
          + SET_LINK_CONTENT + "` command has `" + LINK_CONTENT + "` parameter with not string value, but `"
          + LINK_CONTENT_TYPE + "` parameter is specified as `" + STRING_CONTENT_TYPE + "`."};
      return result;
    }

    result = link.Set(data.get<std::string>());
  }
  else if (contentType == INTEGER_CONTENT_TYPE)
  {
    if (!data.is_number_integer())
    {
      error = {
          "Not able to set content for sc-link because content to be set is specified incorrectly. Payload with `"
          + SET_LINK_CONTENT + "` command has `" + LINK_CONTENT + "` parameter with not integer value, but `"
          + LINK_CONTENT_TYPE + "` parameter is specified as `" + INTEGER_CONTENT_TYPE + "`."};
      return result;
    }

    result = link.Set(data.get<sc_int>());
  }
  else if (contentType == FLOAT_CONTENT_TYPE)
  {
    if (!data.is_number_float())
    {
      error = {
          "Not able to set content for sc-link because content to be set is specified incorrectly. Payload with `"
          + SET_LINK_CONTENT + "` command has `" + LINK_CONTENT + "` parameter with not float value, but `"
          + LINK_CONTENT_TYPE + "` parameter is specified as `" + FLOAT_CONTENT_TYPE + "`."};
      return result;
    }

    result = link.Set(data.get<sc_float>());
  }
  else
  {
    error = {
        "Not able to set content for sc-link because type of content to be set is specified incorrectly. Payload "
        "with `" + SET_LINK_CONTENT + "` command must have `" + LINK_CONTENT_TYPE + "` parameter with one of the "
        "following values: `" + STRING_CONTENT_TYPE + "`, `" + INTEGER_CONTENT_TYPE + "` or `" + FLOAT_CONTENT_TYPE 
        + "`"};
  }

  return result;
}

ScMemoryJsonPayload ScMemoryHandleLinkContentJsonAction::GetLinkContent(
    ScAgentContext * context,
    ScMemoryJsonPayload const & parameters,
    ScMemoryJsonPayload & error)
{
  ScMemoryJsonPayload result;
  if (!parameters.contains(LINK_ADDRESS))
  {
    error = {
        "Not able to get content of sc-link because its sc-address hash is not specified in command. Payload with `"
        + GET_LINK_CONTENT + "` command must have `" + LINK_ADDRESS + "` parameter."};
    return result;
  }

  if (!parameters[LINK_ADDRESS].is_number_integer())
  {
    error = {
        "Not able to get content of sc-link because its sc-address hash is specified incorrectly. Payload with `"
        + GET_LINK_CONTENT + "` command must have `" + LINK_ADDRESS + "` parameter with integer value."};
    return result;
  }

  ScAddr linkAddr{parameters[LINK_ADDRESS].get<size_t>()};
  ScLink link{*context, linkAddr};

  if (link.DetermineType() >= ScLink::Type::Int8 && link.DetermineType() <= ScLink::Type::UInt64)
    result = {{FOUND_LINK_CONTENT, link.Get<sc_int>()}, {LINK_CONTENT_TYPE, INTEGER_CONTENT_TYPE}};
  else if (link.IsType<double>() || link.IsType<float>())
    result = {{FOUND_LINK_CONTENT, link.Get<float>()}, {LINK_CONTENT_TYPE, FLOAT_CONTENT_TYPE}};
  else
    result = {{FOUND_LINK_CONTENT, link.Get<std::string>()}, {LINK_CONTENT_TYPE, STRING_CONTENT_TYPE}};
  return result;
}

ScMemoryJsonPayload ScMemoryHandleLinkContentJsonAction::SearchLinksByContent(
    ScAgentContext * context,
    ScMemoryJsonPayload const & parameters,
    ScMemoryJsonPayload & error)
{
  std::set<size_t> hashes;
  if (!parameters.contains(LINK_CONTENT))
  {
    error = {
        "Not able to search sc-links by content because content to be search is not specified. Payload with `"
        + SEARCH_LINKS_BY_CONTENT + "` command must have `" + LINK_CONTENT + "` parameter."};
    return hashes;
  }

  ScAddrSet linkSet;
  auto const & data = parameters[LINK_CONTENT];
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
        "Not able to search sc-links by content because content to be search is specified incorrectly. Payload with `"
        + SEARCH_LINKS_BY_CONTENT + "` command must have `" + LINK_CONTENT + "` parameter with `" + STRING_CONTENT_TYPE
        + "`, `" + INTEGER_CONTENT_TYPE + "` or `" + FLOAT_CONTENT_TYPE + "` value."};
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
  if (!parameters.contains(LINK_CONTENT_SUBSTRING))
  {
    error = {
        "Not able to search sc-links by content substring because content substring to be search is not specified. "
        "Payload with `"
        + SEARCH_LINKS_BY_CONTENT_SUBSTRING + "` command must have `" + LINK_CONTENT_SUBSTRING + "` parameter."};
    return hashes;
  }

  ScAddrSet linkSet;
  auto const & data = parameters[LINK_CONTENT_SUBSTRING];
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
        "incorrectly. Payload with `" + SEARCH_LINKS_BY_CONTENT_SUBSTRING + "` command must have `" 
        + LINK_CONTENT_SUBSTRING + "` parameter with `" + STRING_CONTENT_TYPE + "`, `" + INTEGER_CONTENT_TYPE + "` "
        "or `" + FLOAT_CONTENT_TYPE + "` value."};
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
  if (!parameters.contains(LINK_CONTENT_SUBSTRING))
  {
    error = {
        "Not able to search contents of sc-links by content substring because content substring to be searched is "
        "not specified. Payload with `"
        + SEARCH_LINKS_CONTENTS_BY_CONTENT_SUBSTRING + "` command must have `" + LINK_CONTENT_SUBSTRING
        + "` parameter."};
    return {};
  }

  auto const & data = parameters[LINK_CONTENT_SUBSTRING];
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
        "specified incorrectly. Payload with `"
        + SEARCH_LINKS_CONTENTS_BY_CONTENT_SUBSTRING + "` command must have `" + LINK_CONTENT_SUBSTRING
        + "` parameter with `" + STRING_CONTENT_TYPE + "`, `" + INTEGER_CONTENT_TYPE + "` or `" + FLOAT_CONTENT_TYPE
        + "` value."};
  }

  return linkContentSet;
}
