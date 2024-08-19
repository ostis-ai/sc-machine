#pragma once

#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <list>

#include <sc-memory/sc_utils.hpp>
#include "gwf_translator_const.hpp"

class SCsTree
{
public:
  SCsTree(std::string const & name, std::string const & content)
    : name(name)
    , content(content)
  {
  }

  static std::shared_ptr<SCsTree> ParseTree(std::string const & input);
  static void PrintTree(std::shared_ptr<SCsTree> const & node, int level = 0);
  static std::shared_ptr<std::list<std::pair<std::string, std::string>>> CompareTrees(
      std::pair<std::shared_ptr<SCsTree>, std::shared_ptr<SCsTree>> const & nodes);
  static void PrintDifferences(std::shared_ptr<std::list<std::pair<std::string, std::string>>> const & differences);

private:
  std::string name;
  std::string content;
  std::map<std::string, std::shared_ptr<SCsTree>> children;

  static std::shared_ptr<SCsTree> AddChildNode(
      std::shared_ptr<SCsTree> parent,
      std::string const & name,
      std::string const & content);
  static bool Check(std::string const & example, std::string const & line);
};
