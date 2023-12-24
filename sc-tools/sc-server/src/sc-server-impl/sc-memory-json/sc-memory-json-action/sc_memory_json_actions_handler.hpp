/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>

#include "../sc_memory_json_handler.hpp"
#include "sc_memory_json_action.hpp"

class ScMemoryJsonActionsHandler : public ScMemoryJsonHandler
{
public:
  explicit ScMemoryJsonActionsHandler(ScServer * server, ScMemoryContext * processCtx);

  ~ScMemoryJsonActionsHandler() override;

private:
  ScMemoryContext * m_context;

  ScMemoryJsonPayload HandleRequestPayload(
      ScServerSessionId const & sessionId,
      std::string const & requestType,
      ScMemoryJsonPayload const & requestPayload,
      ScMemoryJsonPayload & errorsPayload,
      sc_bool & status,
      sc_bool & isEvent) override;

  static std::map<std::string, ScMemoryJsonAction *> m_actions;
};
