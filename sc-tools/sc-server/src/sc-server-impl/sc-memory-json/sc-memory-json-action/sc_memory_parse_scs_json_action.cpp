/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_parse_scs_json_action.hpp"

#include <string>

ScMemoryParseSCsJsonAction::ScMemoryParseSCsJsonAction()
{
  m_parser = new scs::Parser();
}

ScMemoryJsonPayload ScMemoryParseSCsJsonAction::Complete(
    ScAgentContext *,
    ScMemoryJsonPayload requestPayload,
    ScMemoryJsonPayload &)
{
  ScMemoryJsonPayload responsePayload = ScMemoryJsonPayload::array();

  for (auto & atom : requestPayload)
  {
    std::string const & ast = m_parser->BuildAST(atom.get<std::string>());
    responsePayload.push_back(ScMemoryJsonPayload::parse(ast));
  }

  if (responsePayload.is_null())
    return "{}"_json;

  return responsePayload;
}

ScMemoryParseSCsJsonAction::~ScMemoryParseSCsJsonAction()
{
  delete m_parser;
  m_parser = nullptr;
}
