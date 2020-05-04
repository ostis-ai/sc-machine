/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "inference/manager/InferenceManager.hpp"

namespace scAgentsCommon
{

class DirectInferenceManager : InferenceManager
{
public:
  SolutionTreeNode applyInference(
        const ScAddr & targetTemplate,
        const ScAddr & ruleSet,
        const ScAddr & relatedNodeSet) override;
};

} // namespace scAgentsCommon
