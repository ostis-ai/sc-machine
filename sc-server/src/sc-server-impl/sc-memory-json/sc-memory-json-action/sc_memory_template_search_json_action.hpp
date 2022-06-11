#pragma once

#include "sc_memory_make_template_json_action.hpp"

class ScMemoryTemplateSearchJsonAction : public ScMemoryMakeTemplateJsonAction
{
public:
  ScMemoryJsonPayload Complete(ScMemoryContext * context, ScMemoryJsonPayload requestPayload) override
  {
    ScTemplateSearchResult result;
    auto * scTemplate = getTemplate(context, requestPayload);
    context->HelperSearchTemplate(*scTemplate, result);
    delete scTemplate;

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
