/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/cpp/sc_addr.hpp>

namespace scAgentsCommon
{

class SolutionTreeNode
{
private:
  ScAddr node;
  ScAddr previousNode;
  ScAddr usedRule;
  ScAddr result;
  ScAddr relatedNodeSet;

public:
  const ScAddr & getNode() const
  {
    return node;
  }

  void setNode(const ScAddr & node)
  {
    SolutionTreeNode::node = node;
  }

  const ScAddr & getPreviousNode() const
  {
    return previousNode;
  }

  void setPreviousNode(const ScAddr & previousNode)
  {
    SolutionTreeNode::previousNode = previousNode;
  }

  const ScAddr & getUsedRule() const
  {
    return usedRule;
  }

  void setUsedRule(const ScAddr & usedRule)
  {
    SolutionTreeNode::usedRule = usedRule;
  }

  const ScAddr & getResult() const
  {
    return result;
  }

  void setResult(const ScAddr & result)
  {
    SolutionTreeNode::result = result;
  }

  const ScAddr & getRelatedNodeSet() const
  {
    return relatedNodeSet;
  }

  void setRelatedNodeSet(const ScAddr & relatedNodeSet)
  {
    SolutionTreeNode::relatedNodeSet = relatedNodeSet;
  }
};

}