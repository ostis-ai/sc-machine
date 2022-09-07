/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_make_template_json_action.hpp"

class ScMemoryTemplateGenerateJsonAction : public ScMemoryMakeTemplateJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScMemoryContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload & errorsPayload) override
  {
    ScTemplateGenResult result;
    auto const & pair = GetTemplate(context, requestPayload);
    context->HelperGenTemplate(*pair.first, result, pair.second);
    delete pair.first;

    std::vector<size_t> hashesVectors;
    for (size_t i = 0; i < result.Size(); ++i)
      hashesVectors.push_back(result[i].Hash());

    return {{"aliases", result.GetReplacements()}, {"addrs", hashesVectors}};
  }
};
