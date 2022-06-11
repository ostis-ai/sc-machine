#pragma once

#include "sc_memory_make_template_json_action.hpp"

class ScMemoryTemplateGenerateJsonAction : public ScMemoryMakeTemplateJsonAction
{
public:
  ScMemoryJsonPayload Complete(ScMemoryContext * context, ScMemoryJsonPayload requestPayload) override
  {
    ScTemplateGenResult result;
    auto * scTemplate = getTemplate(context, requestPayload);
    context->HelperGenTemplate(*scTemplate, result);
    delete scTemplate;

    std::vector<size_t> hashesVectors;
    for (size_t i = 0; i < result.Size(); ++i)
      hashesVectors.push_back(result[i].Hash());

    return {{"aliases", result.GetReplacements()}, {"addrs", hashesVectors}};
  }
};
