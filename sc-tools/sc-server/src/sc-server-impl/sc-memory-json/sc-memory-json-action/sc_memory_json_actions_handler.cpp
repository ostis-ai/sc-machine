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

std::string ScMemoryJsonActionsHandler::Handle(
    ScServerConnectionHandle const & hdl,
    std::string const & requestMessage)
{
  std::vector<ScMemoryJsonPayload> requestData = ParseRequestMessage(requestMessage);
  if (requestData.empty())
    return "Invalid request message";

  std::string const & requestType = requestData.at(0).get<std::string>();
  ScMemoryJsonPayload const & requestPayload = requestData.at(1);
  size_t const & requestId = requestData.at(2).get<size_t>();

  sc_bool status = SC_FALSE;
  ScMemoryJsonPayload const & responsePayload = HandleRequestPayload(requestType, requestPayload, status);

  return GenerateResponseText(requestId, status, responsePayload);
}

std::vector<ScMemoryJsonPayload> ScMemoryJsonActionsHandler::ParseRequestMessage(std::string const & requestMessage)
{
  std::vector<ScMemoryJsonPayload> requestData;

  ScMemoryJsonPayload const & messageJson = JsonifyRequestMessage(requestMessage);
  if (messageJson.is_null())
    return requestData;

  requestData = {messageJson["type"], messageJson["payload"], messageJson["id"]};

  for (auto const & item : requestData)
  {
    if (item.is_null())
      return {};
  }

  return requestData;
}

ScMemoryJsonPayload ScMemoryJsonActionsHandler::JsonifyRequestMessage(std::string const & requestMessage)
{
  ScMemoryJsonPayload json;
  try
  {
    json = ScMemoryJsonPayload::parse(requestMessage);
  }
  catch (ScMemoryJsonPayload::parse_error const & e)
  {
    m_server->LogError(ScServerLogErrors::rerror, e.what());
  }

  return json;
}

ScMemoryJsonPayload ScMemoryJsonActionsHandler::HandleRequestPayload(
    std::string const & requestType, ScMemoryJsonPayload const & requestPayload, sc_bool & result)
{
  result = SC_FALSE;

  ScMemoryJsonPayload responsePayload;
  auto const & it = m_actions.find(requestType);
  if (it == m_actions.end())
  {
    responsePayload = "Unsupported request type: " + requestType;
    m_server->LogError(ScServerLogErrors::rerror, responsePayload);
    return responsePayload;
  }

  try
  {
    auto * action = it->second;
    responsePayload = action->Complete(m_context, requestPayload);

    result = SC_TRUE;
  }
  catch (ScServerException const & e)
  {
    responsePayload = e.m_msg;
  }
  catch (utils::ScException const & e)
  {
    responsePayload = e.Description();
  }
  catch (std::exception const & e)
  {
    responsePayload = e.what();
  }
  catch (...)
  {
    responsePayload = "Undefined error occurred";
  }

  if (result == SC_FALSE)
  {
    m_server->LogError(ScServerLogErrors::rerror, responsePayload);
  }

  return responsePayload;
}

std::string ScMemoryJsonActionsHandler::GenerateResponseText(
    size_t requestId,
    sc_bool status,
    ScMemoryJsonPayload const & responsePayload)
{
  return ScMemoryJsonPayload({{"id", requestId}, {"event", SC_FALSE}, {"status", status}, {"payload", responsePayload}})
      .dump();
}
