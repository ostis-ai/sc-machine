/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/cpp/kpm/sc_agent.hpp>

#include "inference/model/SolutionTreeNode.hpp"

using namespace std;

namespace scAgentsCommon
{

class InferenceManager
{
public:
  virtual SolutionTreeNode applyInference(
        const ScAddr & targetTemplate,
        const ScAddr & ruleSet,
        const ScAddr & relatedNodeSet);
};

} // namespace scAgentsCommon
