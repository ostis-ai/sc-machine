/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>

#include "sc_memory_json_payload.hpp"

#include "../sc_server_defines.hpp"
#include "../sc_server.hpp"

class ScMemoryJsonHandler
{
public:
  explicit ScMemoryJsonHandler(ScServer * server)
    : m_server(server)
  {
  }

  virtual ~ScMemoryJsonHandler() = default;

  virtual std::string Handle(ScServerSessionId const & sessionId, std::string const & requestMessage);

protected:
  ScServer * m_server;

  std::vector<ScMemoryJsonPayload> ParseRequestMessage(std::string const & requestMessage);

  ScMemoryJsonPayload JsonifyRequestMessage(std::string const & requestMessage);

  virtual ScMemoryJsonPayload ResponseRequestMessage(
      ScServerSessionId const & sessionId,
      size_t requestId,
      std::string const & requestType,
      ScMemoryJsonPayload const & requestPayload);

  virtual ScMemoryJsonPayload HandleRequestPayload(
      ScServerSessionId const & sessionId,
      std::string const & requestType,
      ScMemoryJsonPayload const & requestPayload,
      ScMemoryJsonPayload & errorsPayload,
      sc_bool & status,
      sc_bool & isEvent) = 0;

  static ScMemoryJsonPayload FormResponseMessage(
      size_t requestId,
      sc_bool event,
      sc_bool status,
      ScMemoryJsonPayload const & errorsPayload,
      ScMemoryJsonPayload const & responsePayload);
};
