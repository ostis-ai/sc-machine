/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

#include <sc-memory/sc_type.hpp>

class ScMemoryHandleLinkContentJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScAgentContext * context,
      ScMemoryJsonPayload requestPayload,
      ScMemoryJsonPayload & errorsPayload) override;

private:
  ScMemoryJsonPayload SetLinkContent(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  ScMemoryJsonPayload GetLinkContent(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  ScMemoryJsonPayload SearchLinksByContent(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  ScMemoryJsonPayload SearchLinksByContentSubstring(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  ScMemoryJsonPayload SearchLinksContentsByContentSubstring(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  sc_uint32 const maxLengthToSearchAsPrefix = 4;

  inline static std::string const HANDLE_LINK_CONTENT = "content";
  inline static std::string const SET_LINK_CONTENT = "set";
  inline static std::string const GET_LINK_CONTENT = "get";
  inline static std::string const SEARCH_LINKS_BY_CONTENT = "find";
  inline static std::string const SEARCH_LINKS_BY_CONTENT_SUBSTRING = "find_links_by_substr";
  inline static std::string const SEARCH_LINKS_CONTENTS_BY_CONTENT_SUBSTRING = "find_strings_by_substr";

  inline static std::string const COMMAND = "command";
  inline static std::string const LINK_ADDRESS = "addr";
  inline static std::string const LINK_CONTENT_TYPE = "type";
  inline static std::string const LINK_CONTENT = "data";
  inline static std::string const FOUND_LINK_CONTENT = "value";
  inline static std::string const LINK_CONTENT_SUBSTRING = "data";

  inline static std::string const STRING_CONTENT_TYPE = "string";
  inline static std::string const INTEGER_CONTENT_TYPE = "int";
  inline static std::string const FLOAT_CONTENT_TYPE = "float";
};
