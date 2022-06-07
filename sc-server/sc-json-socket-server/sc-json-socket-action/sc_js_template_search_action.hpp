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

    std::vector<std::vector<size_t>> hashesVectors;
    for (size_t i = 0; i < result.Size(); ++i)
    {
      auto const & item = result[i];

      std::vector<size_t> hashesTriple{};
      for (size_t j = 0; j < result.Size(); ++j)
        hashesTriple.push_back(item[j].Hash());

      hashesVectors.push_back(hashesTriple);
    }
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
