/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>

#include "sc_memory_json_action_defines.hpp"
#include "../sc_memory_json_handler.hpp"

class ScMemoryJsonActionsHandler : public ScMemoryJsonHandler
{
public:
  ScMemoryJsonActionsHandler()
  {
    m_context = new ScMemoryContext("sc-json-socket-actions-handler");
  }

  std::string Handle(ScServerConnectionHandle const & hdl, std::string const & requestMessageText) override;

  ~ScMemoryJsonActionsHandler() override
  {
    m_context->Destroy();
    delete m_context;

    for (auto const & it : m_actions)
      delete it.second;
  }

private:
  ScMemoryContext * m_context;

  std::map<std::string, ScMemoryJsonAction *> m_actions = {
      {"keynodes", new ScMemoryHandleKeynodesJsonAction()},
      {"create_elements", new ScMemoryCreateElementsJsonAction()},
      {"create_elements_by_scs", new ScMemoryCreateElementsByScsJsonAction()},
      {"check_elements", new ScMemoryCheckElementsJsonAction()},
      {"delete_elements", new ScMemoryDeleteElementsJsonAction()},
      {"search_template", new ScMemoryTemplateSearchJsonAction()},
      {"generate_template", new ScMemoryTemplateGenerateJsonAction()},
      {"content", new ScMemoryHandleLinkContentJsonAction()},
  };

  static std::string GenerateResponseText(
      size_t requestId,
      sc_bool status,
      ScMemoryJsonPayload const & responsePayload);
};
