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

using SCsTreePtr = std::shared_ptr<class SCsTree>;

class SCsTree
{
public:
  SCsTree(std::string const & name, std::string const & content);

  static SCsTreePtr ParseTree(std::string const & input);
  static void PrintTree(SCsTreePtr const & node, int level = 0);
  static std::shared_ptr<std::list<std::pair<std::string, std::string>>> CompareTrees(
      std::pair<SCsTreePtr, SCsTreePtr> const & nodes);
  static void PrintDifferences(std::shared_ptr<std::list<std::pair<std::string, std::string>>> const & differences);

private:
  std::string name;
  std::string content;
  std::map<std::string, SCsTreePtr> children;

  static SCsTreePtr AddChildNode(
      SCsTreePtr parent,
      std::string const & name,
      std::string const & content);
  static bool Check(std::string const & example, std::string const & line);
};
