/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_template_generate_json_action.hpp"

ScMemoryJsonPayload ScMemoryTemplateGenerateJsonAction::Complete(
    ScMemoryContext * context,
    ScMemoryJsonPayload requestPayload,
    ScMemoryJsonPayload & errorsPayload)
{
  ScTemplateGenResult result;
  auto pair = GetTemplate(context, requestPayload);
  context->HelperGenTemplate(*pair.first, result, pair.second);

  std::vector<size_t> hashesVectors;
  for (size_t i = 0; i < result.Size(); ++i)
    hashesVectors.push_back(result[i].Hash());

  ScMemoryJsonPayload const & resultPayload = {{"aliases", result.GetReplacements()}, {"addrs", hashesVectors}};
  delete pair.first;
  return resultPayload;
}
