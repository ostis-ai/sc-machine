/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_json_actions_handler.hpp"

ScMemoryJsonActionsHandler::ScMemoryJsonActionsHandler(ScServer * server)
  : ScMemoryJsonHandler(server)
{
  m_context = new ScMemoryContext("sc-json-socket-actions-handler");
}

ScMemoryJsonActionsHandler::~ScMemoryJsonActionsHandler()
{
  m_context->Destroy();
  delete m_context;

  for (auto const & it : m_actions)
    delete it.second;
  m_actions.clear();
}

ScMemoryJsonPayload ScMemoryJsonActionsHandler::HandleRequestPayload(
    ScServerConnectionHandle const & hdl,
    std::string const & requestType,
    ScMemoryJsonPayload const & requestPayload,
    ScMemoryJsonPayload & errorsPayload,
    sc_bool & status,
    sc_bool & isEvent)
{
  status = SC_FALSE;
  isEvent = SC_FALSE;

  ScMemoryJsonPayload responsePayload;
  auto const & it = m_actions.find(requestType);
  if (it == m_actions.end())
  {
    errorsPayload = "Unsupported request type: " + requestType;
    m_server->LogError(ScServerLogErrors::rerror, errorsPayload);
    return responsePayload;
  }

  auto * action = it->second;
  responsePayload = action->Complete(m_context, requestPayload, errorsPayload);

  status = errorsPayload.empty();
  return responsePayload;
}
