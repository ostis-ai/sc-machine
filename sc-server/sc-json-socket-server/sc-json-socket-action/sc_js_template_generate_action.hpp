#pragma once

#include "sc_js_make_template_action.hpp"

class ScJSTemplateGenerateAction : public ScJSMakeTemplateAction
{
public:
  ScJSPayload Complete(ScMemoryContext * context, ScJSPayload requestPayload) override
  {
    ScTemplateGenResult result;
    auto * scTemplate = getTemplate(context, requestPayload);
    context->HelperGenTemplate(*scTemplate, result);

    std::vector<size_t> hashesVectors;
    for (size_t i = 0; i < result.Size(); ++i)
      hashesVectors.push_back(result[i].Hash());

    delete scTemplate;

    auto const & replacements = result.GetReplacements();
    std::set<std::string> aliases;
    std::transform(
        replacements.begin(), replacements.end(), std::inserter(aliases, aliases.end()), [](auto const & pair) {
          return pair.first;
        });

    return {{"aliases", aliases}, {"addrs", hashesVectors}};
  }
};
