/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_make_template_json_action.hpp"

class ScMemoryTemplateSearchJsonAction : public ScMemoryMakeTemplateJsonAction
{
public:
  ScMemoryJsonPayload Complete(
      ScMemoryContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload & errorsPayload) override
  {
    ScTemplateSearchResult result;
    auto const & pair = GetTemplate(context, requestPayload);
    context->HelperSearchTemplate(*pair.first, result);
    delete pair.first;

    std::vector<std::vector<size_t>> hashesVectors;
    for (size_t i = 0; i < result.Size(); ++i)
    {
      auto const & item = result[i];

      std::vector<size_t> vector;
      for (size_t j = 0; j != item.Size(); ++j)
        vector.push_back(item[j].Hash());

      hashesVectors.push_back(vector);
    }

    return {{"aliases", result.GetReplacements()}, {"addrs", hashesVectors}};
  }
};
