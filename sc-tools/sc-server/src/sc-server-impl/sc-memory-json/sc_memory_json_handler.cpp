/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_json_handler.hpp"

std::string ScMemoryJsonHandler::Handle(ScServerConnectionHandle const & hdl, std::string const & requestMessage)
{
  std::vector<ScMemoryJsonPayload> requestData = ParseRequestMessage(requestMessage);
  if (requestData.empty())
    return "Invalid request message";

  std::string const & requestType = requestData.at(0).get<std::string>();
  ScMemoryJsonPayload const & requestPayload = requestData.at(1);
  size_t const & requestId = requestData.at(2).get<size_t>();

  return ResponseRequestMessage(hdl, requestId, requestType, requestPayload).dump();
}

std::vector<ScMemoryJsonPayload> ScMemoryJsonHandler::ParseRequestMessage(std::string const & requestMessage)
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

ScMemoryJsonPayload ScMemoryJsonHandler::JsonifyRequestMessage(std::string const & requestMessage)
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

ScMemoryJsonPayload ScMemoryJsonHandler::ResponseRequestMessage(
    ScServerConnectionHandle const & hdl,
    size_t const requestId,
    std::string const & requestType,
    ScMemoryJsonPayload const & requestPayload)
{
  sc_bool status = SC_FALSE;

  sc_bool isEvent = SC_FALSE;
  ScMemoryJsonPayload responsePayload;
  ScMemoryJsonPayload errorsPayload = ScMemoryJsonPayload::array({});
  try
  {
    responsePayload = HandleRequestPayload(hdl, requestType, requestPayload, errorsPayload, status, isEvent);
  }
  catch (ScServerException const & e)
  {
    errorsPayload = e.m_msg;
  }
  catch (utils::ScException const & e)
  {
    errorsPayload = e.Description();
  }
  catch (std::exception const & e)
  {
    errorsPayload = e.what();
  }
  catch (...)
  {
    errorsPayload = "Undefined error occurred";
  }

  if (status == SC_FALSE && errorsPayload.is_string())
  {
    m_server->LogError(ScServerLogErrors::rerror, errorsPayload.get<std::string>());
  }

  return FormResponseMessage(requestId, isEvent, status, errorsPayload, responsePayload);
}

ScMemoryJsonPayload ScMemoryJsonHandler::FormResponseMessage(
    size_t requestId,
    sc_bool event,
    sc_bool status,
    ScMemoryJsonPayload const & errorsPayload,
    ScMemoryJsonPayload const & responsePayload)
{
  return ScMemoryJsonPayload({{"id", requestId}, {"event", event}, {"status", status}, {"errors", errorsPayload}, {"payload", responsePayload}});
}
