/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

#include <sc-memory/sc_type.hpp>

/*!
 * @class ScMemoryHandleLinkContentJsonAction
 * @brief Handles actions related to sc-link content management via JSON commands.
 *
 * This class provides methods for completing various commands related to sc-links, including
 * setting and getting link content, as well as searching for links by their content or
 * substrings of their content. It processes JSON payloads to execute the appropriate actions
 * and returns results or errors as necessary.
 *
 * @code
 * // payload grammar
 * sc_json_command_handle_link_contents
 * : '"type"' ':' '"content"' ','
 *   '"payload"' ':'
 *   '['(
 *       '{'
 *            '"command"' ':' '"set"' ','
 *            '"type"' ':' SC_LINK_CONTENT_TYPE ','
 *            '"data"' ':' NUMBER_CONTENT | STRING_CONTENT ','
 *            '"addr"' ':' SC_ADDR_HASH ','
 *       '}' ','
 *       |
 *       '{'
 *            '"command' ':' '"get"' ','
 *            '"addr"' ':' SC_ADDR_HASH ','
 *       '}' ','
 *       |
 *       '{'
 *            '"command' ':' '"find"' ','
 *            '"data"' ':' NUMBER_CONTENT | STRING_CONTENT ','
 *       '}' ','
 *       |
 *       '{'
 *            '"command' ':' '"find_links_by_substr"' ','
 *            '"data"' ':' NUMBER_CONTENT | STRING_CONTENT ','
 *        '}' ','
 *       |
 *       '{'
 *            '"command' ':' '"find_strings_by_substr"' ','
 *            '"data"' ':' NUMBER_CONTENT | STRING_CONTENT ','
 *       '}' ','
 *   )*']' ','
 * ;
 * @endcode
 *
 * @code
 * // result grammar
 * sc_json_command_answer_handle_link_contents
 * : '"payload"' ':'
 *   '['(
 *       BOOL
 *       |
 *       '{'
 *           '"value"' ':' NUMBER_CONTENT | STRING_CONTENT  ','
 *           '"type"' ':' SC_LINK_CONTENT_TYPE ','
 *       '}' ','
 *       |
 *       '['
 *           (SC_ADDR_HASH ',')*
 *       ']' ','
 *   )*']' ','
 * ;
 * @endcode
 */
class ScMemoryHandleLinkContentJsonAction : public ScMemoryJsonAction
{
public:
  /*!
   * Completes a command related to sc-links based on the provided JSON payload.
   *
   * @param context A pointer to the ScAgentContext providing the execution context.
   * @param requestPayload A JSON payload containing the command to be executed.
   * @param errorsPayload A reference to a JSON payload where any errors encountered will be stored.
   * @return A JSON payload containing the result of the command execution.
   */
  ScMemoryJsonPayload Complete(
      ScAgentContext * context,
      ScMemoryJsonPayload requestPayload,
      ScMemoryJsonPayload & errorsPayload) override;

private:
  /*!
   * Sets the content of a specified sc-link based on the provided parameters.
   *
   * @param context A pointer to the ScAgentContext providing the execution context.
   * @param parameters A JSON payload containing parameters for setting link content.
   * @param error A reference to a JSON payload where any errors encountered will be stored.
   * @return A boolean indicating whether the content was successfully set.
   */
  ScMemoryJsonPayload SetLinkContent(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  /*!
   * Retrieves the content of a specified sc-link based on the provided parameters.
   *
   * @param context A pointer to the ScAgentContext providing the execution context.
   * @param parameters The JSON payload containing parameters for getting link content.
   * @param error A reference to a JSON payload where any errors encountered will be stored.
   * @return A JSON payload containing the content of the specified sc-link.
   */
  ScMemoryJsonPayload GetLinkContent(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  /*!
   * Searches for sc-links by their content based on the provided parameters.
   *
   * @param context A pointer to the ScAgentContext providing the execution context.
   * @param parameters The JSON payload containing parameters for searching links by content.
   * @param error A reference to a JSON payload where any errors encountered will be stored.
   * @return A set of hashes representing the found sc-links.
   */
  ScMemoryJsonPayload SearchLinksByContent(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  /*!
   * Searches for sc-links by a substring of their content based on the provided parameters.
   *
   * @param context A pointer to the ScAgentContext providing the execution context.
   * @param parameters A JSON payload containing parameters for searching links by content substring.
   * @param error A reference to a JSON payload where any errors encountered will be stored.
   * @return A set of hashes representing the found sc-links that match the substring criteria.
   */
  ScMemoryJsonPayload SearchLinksByContentSubstring(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  /*!
   * Searches for contents of sc-links by a substring of their content based on the provided parameters.
   *
   * @param context A pointer to the ScAgentContext providing the execution context.
   * @param parameters A JSON payload containing parameters for searching contents by content substring.
   * @param error A reference to a JSON payload where any errors encountered will be stored.
   * @return A set of strings representing the contents of found sc-links that match the substring criteria.
   */
  ScMemoryJsonPayload SearchLinksContentsByContentSubstring(
      ScAgentContext * context,
      ScMemoryJsonPayload const & parameters,
      ScMemoryJsonPayload & error);

  sc_uint32 const maxLengthToSearchAsPrefix = 4;

  // handle link content commands
  inline static std::string const HANDLE_LINK_CONTENT = "content";
  inline static std::string const SET_LINK_CONTENT = "set";
  inline static std::string const GET_LINK_CONTENT = "get";
  inline static std::string const SEARCH_LINKS_BY_CONTENT = "find";
  inline static std::string const SEARCH_LINKS_BY_CONTENT_SUBSTRING = "find_links_by_substr";
  inline static std::string const SEARCH_LINKS_CONTENTS_BY_CONTENT_SUBSTRING = "find_strings_by_substr";

  // payload parameters
  inline static std::string const COMMAND = "command";
  inline static std::string const LINK_ADDRESS = "addr";
  inline static std::string const LINK_CONTENT_TYPE = "type";
  inline static std::string const LINK_CONTENT = "data";
  inline static std::string const LINK_CONTENT_SUBSTRING = "data";

  // result parameters
  inline static std::string const FOUND_LINK_CONTENT = "value";

  // link content types
  inline static std::string const STRING_CONTENT_TYPE = "string";
  inline static std::string const INTEGER_CONTENT_TYPE = "int";
  inline static std::string const FLOAT_CONTENT_TYPE = "float";
};
