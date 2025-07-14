/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

#include <sc-memory/scs/scs_parser.hpp>

/*!
 * @brief An action handler that parses SCs text input into an Abstract Syntax Tree (AST) represented as JSON.
 *
 * This class extends ScMemoryJsonAction to provide functionality for handling requests that contain sc.s-text.
 * It uses an internal scs::Parser instance to convert the input sc.s-text
 * into JSON-formatted ASTs.
 */
class ScMemoryParseSCsJsonAction : public ScMemoryJsonAction
{
public:
  /*!
   * @brief Constructs the action handler and initializes the SCs parser.
   */
  ScMemoryParseSCsJsonAction();

  /*!
   * @brief Executes the parsing action.
   *
   * Takes a JSON payload containing an array of strings (SCs text snippets),
   * parses each snippet using the scs::Parser, and returns a JSON payload
   * containing an array of the resulting ASTs (also as JSON).
   *
   * @param context Pointer to the agent context (unused in this implementation).
   * @param requestPayload Input JSON payload. Expected to be an array of strings,
   *                       where each string is SCs code.
   * @param errorsPayload Output JSON payload for reporting errors (unused in this implementation).
   * @return ScMemoryJsonPayload A JSON array containing the JSON representation of the AST
   *                             for each successfully parsed input string. Returns an empty JSON object "{}"
   *                             if the resulting array would be null (which is unlikely with current logic).
   */
  ScMemoryJsonPayload Complete(
      ScAgentContext * context,
      ScMemoryJsonPayload requestPayload,
      ScMemoryJsonPayload & errorsPayload) override;

  /*!
   * @brief Destroys the action handler and cleans up the SCs parser instance.
   */
  ~ScMemoryParseSCsJsonAction() override;

private:
  scs::Parser * m_parser;  //!< Pointer to the SCs parser instance used by this action.
};
