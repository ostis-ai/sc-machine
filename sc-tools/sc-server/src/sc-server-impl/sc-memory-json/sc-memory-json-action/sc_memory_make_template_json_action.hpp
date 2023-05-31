/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryMakeTemplateJsonAction : public ScMemoryJsonAction
{
protected:
  std::pair<ScTemplate *, ScTemplateParams> GetTemplate(ScMemoryContext * context, ScMemoryJsonPayload payload);

  ScTemplate * MakeTemplate(ScMemoryJsonPayload const & triples);
};
