/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "DirectInferenceManager.hpp"

using namespace scAgentsCommon;

SolutionTreeNode DirectInferenceManager::applyInference(
      const ScAddr & targetTemplate,
      const ScAddr & ruleSet,
      const ScAddr & relatedNodeSet)
{

  return InferenceManager::applyInference(targetTemplate, ruleSet, relatedNodeSet);
}
