/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_json_actions_handler.hpp"

#include "sc_memory_json_action_defines.hpp"

std::map<std::string, ScMemoryJsonAction *> ScMemoryJsonActionsHandler::m_actions;

ScMemoryJsonActionsHandler::ScMemoryJsonActionsHandler(ScServer * server, ScAgentContext * sessionCtx)
  : ScMemoryJsonHandler(server)
  , m_context(sessionCtx)
{
}

ScMemoryJsonActionsHandler::~ScMemoryJsonActionsHandler() = default;

void ScMemoryJsonActionsHandler::InitializeActionClasses()
{
  m_actions = {
      {"connection_info", new ScMemoryConnectionInfoJsonAction()},
      {"keynodes", new ScMemoryHandleKeynodesJsonAction()},
      {"create_elements", new ScMemoryGenerateElementsJsonAction()},
      {"create_elements_by_scs", new ScMemoryGenerateElementsByScsJsonAction()},
      {"check_elements", new ScMemoryCheckElementsJsonAction()},
      {"delete_elements", new ScMemoryEraseElementsJsonAction()},
      {"search_template", new ScMemoryTemplateSearchJsonAction()},
      {"generate_template", new ScMemoryTemplateGenerateJsonAction()},
      {"content", new ScMemoryHandleLinkContentJsonAction()},
  };
}

void ScMemoryJsonActionsHandler::ClearActionClasses()
{
  for (auto & it : m_actions)
  {
    delete it.second;
    it.second = nullptr;
  }
}

ScMemoryJsonPayload ScMemoryJsonActionsHandler::HandleRequestPayload(
    ScServerSessionId const &,
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
    m_server->LogMessage(ScServerErrorLevel::error, errorsPayload);
    return responsePayload;
  }

  auto * action = it->second;
  responsePayload = action->Complete(m_context, requestPayload, errorsPayload);

  status = errorsPayload.empty();
  return responsePayload;
}
