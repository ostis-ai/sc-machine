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
  explicit ScMemoryJsonActionsHandler(ScServer * server);

  ~ScMemoryJsonActionsHandler() override;

private:
  ScMemoryContext * m_context;

  ScMemoryJsonPayload HandleRequestPayload(
      ScServerConnectionHandle const & hdl,
      std::string const & requestType,
      ScMemoryJsonPayload const & requestPayload,
      ScMemoryJsonPayload & errorsPayload,
      sc_bool & status,
      sc_bool & isEvent) override;

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
};
