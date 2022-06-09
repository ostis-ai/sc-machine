#pragma once

#include "sc_js_make_template_action.hpp"

class ScJSTemplateSearchAction : public ScJSMakeTemplateAction
{
public:
  ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) override
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

    return {{"aliases",  result.GetReplacements()}, {"addrs", hashesVectors}};
  }
};
