/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <list>

#include <sc-memory/sc_utils.hpp>

#include "gwf_translator_constants.hpp"

using SCsTreeNodePtr = std::shared_ptr<class SCsTreeNode>;
using SCsTreePtr = std::shared_ptr<class SCsTree>;

class SCsTreeNode
{
  friend class SCsTree;

public:
  SCsTreeNode(std::string const & name, std::string const & content);

  std::shared_ptr<std::list<std::pair<std::string, std::string>>> Compare(SCsTreeNode const & otherNode);

private:
  std::string m_name;
  std::string m_content;
  std::map<std::string, SCsTreeNodePtr> children;

  SCsTreeNodePtr AddChildNode(std::string const & name, std::string const & content);
};

class SCsTree
{
public:
  void Parse(std::string const & scsText);

  std::shared_ptr<std::list<std::pair<std::string, std::string>>> Compare(SCsTree const & otherTree);

private:
  SCsTreeNodePtr m_root;

  static bool Check(std::string const & example, std::string const & line);
};
