/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

#include "sc-memory/scs/scs_parser.hpp"

class ScMemoryParseSCsJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryParseSCsJsonAction()
  {
    m_parser = new scs::Parser();
  }

  ScMemoryJsonPayload Complete(
      ScAgentContext * context,
      ScMemoryJsonPayload requestPayload,
      ScMemoryJsonPayload & errorsPayload) override
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

  ~ScMemoryParseSCsJsonAction() override
  {
    delete m_parser;
  }

private:
  scs::Parser * m_parser;
};
