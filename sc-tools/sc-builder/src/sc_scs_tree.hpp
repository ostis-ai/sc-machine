#pragma once

#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

std::string const EQUAL = "=";
std::string const BLOCK_BEGIN = "[";
std::string const BLOCK_END = "];;";
std::string const EDGE_BEGIN = "(";
std::string const EDGE_END = ");;";
std::string const CONTOUR_BEGIN = "[*";
std::string const CONTOUR_END = "*];;";
std::string const ELEMENT_END = ";;";

class ScsTree
{
public:
  ScsTree(std::string const & name, std::string const & content)
    : name(name)
    , content(content)
  {
  }

  static std::shared_ptr<ScsTree> ParseTree(std::string const & input);
  static void PrintTree(std::shared_ptr<ScsTree> const & node, int level = 0);
  static std::shared_ptr<std::list<std::pair<std::string, std::string>>> CompareTrees(
      std::pair<std::shared_ptr<ScsTree>, std::shared_ptr<ScsTree>> const & nodes);
  static void PrintDifferences(std::shared_ptr<std::list<std::pair<std::string, std::string>>> const & differences);

private:
  std::string name;
  std::string content;
  std::map<std::string, std::shared_ptr<ScsTree>> children;

  static std::shared_ptr<ScsTree> AddChildNode(
      std::shared_ptr<ScsTree> parent,
      std::string const & name,
      std::string const & content);
  static std::string Trim(std::string const & str);
  static bool Check(std::string const & example, std::string const & line);
};

std::shared_ptr<ScsTree> ScsTree::AddChildNode(
    std::shared_ptr<ScsTree> parent,
    std::string const & name,
    std::string const & content)
{
  auto node = std::make_shared<ScsTree>(name, content);
  if (parent)
  {
    parent->children.emplace(name, node);
  }
  return node;
}

std::string ScsTree::Trim(std::string const & str)
{
  auto start = str.find_first_not_of(" \t");
  if (start == std::string::npos)
  {
    return "";
  }

  auto end = str.find_last_not_of(" \t");
  return str.substr(start, end - start + 1);
}

inline bool ScsTree::Check(std::string const & example, std::string const & line)
{
  return line.find(example) != std::string::npos;
}

std::shared_ptr<ScsTree> ScsTree::ParseTree(std::string const & input)
{
  std::istringstream iss(input);
  std::string line;
  std::list<std::shared_ptr<ScsTree>> nodeStack;
  auto root = std::make_shared<ScsTree>("root", "");
  auto currentNode = root;
  bool isContentMultiLine = false;
  bool flag = false;
  std::string multiLineContent;
  std::string currentName;

  while (std::getline(iss, line))
  {
    line = Trim(line);
    if (line.empty())
      continue;

    if (isContentMultiLine)
    {
      if (Check(BLOCK_END, line))
      {
        size_t blockEndPos = line.find(BLOCK_END);
        multiLineContent += "\n" + line.substr(0, blockEndPos);

        currentNode = AddChildNode(nodeStack.empty() ? root : nodeStack.back(), currentName, multiLineContent);
        isContentMultiLine = false;
        multiLineContent.clear();

        continue;
      }

      multiLineContent += "\n" + line;

      continue;
    }
    else if (flag)
    {
      size_t elementEndPos = line.find(ELEMENT_END);

      std::string const content = Trim(line.substr(0, elementEndPos));

      currentNode = AddChildNode(nodeStack.empty() ? root : nodeStack.back(), currentName, content);

      flag = false;
      continue;
    }

    if (Check(EQUAL, line))
    {
      size_t equalPos = line.find(EQUAL);
      std::string const name = Trim(line.substr(0, equalPos));

      if (Check(EDGE_BEGIN, line))
      {
        size_t edgeBeginPos = line.find(EDGE_BEGIN) + EDGE_BEGIN.length();

        if (Check(EDGE_END, line))
        {
          size_t edgeEndPos = line.find(EDGE_END);

          std::string const content = Trim(line.substr(edgeBeginPos, edgeEndPos - edgeBeginPos));

          currentNode = AddChildNode(nodeStack.empty() ? root : nodeStack.back(), name, content);

          continue;
        }
        else
        {
          currentName = name;

          std::string const content = Trim(line.substr(edgeBeginPos));

          multiLineContent = content;
          isContentMultiLine = true;
        }
      }
      else if (Check(CONTOUR_BEGIN, line))
      {
        currentNode = AddChildNode(nodeStack.empty() ? root : nodeStack.back(), name, "");
        nodeStack.push_back(currentNode);
      }
      else if (Check(BLOCK_BEGIN, line))
      {
        size_t blockBeginPos = line.find(BLOCK_BEGIN) + BLOCK_BEGIN.length();

        if (Check(BLOCK_END, line))
        {
          size_t blockEndPos = line.find(BLOCK_END);

          std::string const content = Trim(line.substr(blockBeginPos, blockEndPos - blockBeginPos));

          currentNode = AddChildNode(nodeStack.empty() ? root : nodeStack.back(), name, content);

          continue;
        }
        else
        {
          currentName = name;

          std::string const content = Trim(line.substr(blockBeginPos));

          multiLineContent = content;
          isContentMultiLine = true;
        }
      }
    }
    else if (Check(CONTOUR_END, line))
    {
      nodeStack.pop_back();
      currentNode = nodeStack.empty() ? root : nodeStack.back();
    }
    else if (Check(ELEMENT_END, line))
    {
      size_t elementEndPos = line.find(ELEMENT_END);

      std::string const name = Trim(line.substr(0, elementEndPos));
      currentNode = AddChildNode(nodeStack.empty() ? root : nodeStack.back(), name, "");
    }
    else
    {
      currentName = Trim(line);

      // Use flag to correct process elements like:
      // ..el_102182951392320
      //  => nrel_main_idtf: [ostis];;

      flag = true;
    }
  }

  return root;
}

void ScsTree::PrintTree(std::shared_ptr<ScsTree> const & node, int level)
{
  if (!node)
    return;
  std::cout << std::string(level, '\t') << node->name << ": " << node->content << std::endl;
  for (auto const & child : node->children)
  {
    PrintTree(child.second, level + 1);
  }
}

void ScsTree::PrintDifferences(std::shared_ptr<std::list<std::pair<std::string, std::string>>> const & differences)
{
  if (differences == nullptr)
  {
    std::cout << "Trees are identical" << std::endl;
    return;
  }

  std::cout << "Trees differ in the following elements:" << std::endl;
  for (auto const & diff : *differences)
  {
    std::cout << "Difference: " << diff.first << " vs " << diff.second << std::endl;
  }
}

std::shared_ptr<std::list<std::pair<std::string, std::string>>> ScsTree::CompareTrees(
    std::pair<std::shared_ptr<ScsTree>, std::shared_ptr<ScsTree>> const & nodes)
{
  auto differences = std::make_shared<std::list<std::pair<std::string, std::string>>>();

  auto const & node1 = nodes.first;
  auto const & node2 = nodes.second;

  if (!node1 && !node2)
    return differences;

  if (!node1 || !node2)
  {
    differences->emplace_back(node1 ? node1->name : "null", node2 ? node2->name : "null");
    return differences;
  }

  if (node1->name != node2->name)
  {
    differences->emplace_back(node1->name, node2->name);
  }

  if (node1->content != node2->content)
  {
    differences->emplace_back(node1->content, node2->content);
  }

  if (node1->children.size() != node2->children.size())
  {
    differences->emplace_back(
        "children size " + std::to_string(node1->children.size()),
        "children size " + std::to_string(node2->children.size()));
  }

  for (auto const & child1 : node1->children)
  {
    auto it = node2->children.find(child1.first);
    if (it == node2->children.end())
    {
      differences->emplace_back(child1.first, "missing");
    }
    else
    {
      auto childDifferences = ScsTree::CompareTrees({child1.second, it->second});
      if (childDifferences)
      {
        differences->insert(differences->end(), childDifferences->begin(), childDifferences->end());
      }
    }
  }

  for (auto const & child2 : node2->children)
  {
    if (node1->children.find(child2.first) == node1->children.end())
    {
      differences->emplace_back("missing in node1", child2.first);
    }
  }

  return differences;
}
