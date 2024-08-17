#pragma once

#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

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
  static std::shared_ptr<std::vector<std::pair<std::string, std::string>>> CompareTrees(
      std::shared_ptr<ScsTree> const & node1,
      std::shared_ptr<ScsTree> const & node2);
  static void PrintDifferences(std::shared_ptr<std::vector<std::pair<std::string, std::string>>> const & differences);

private:
  std::string const EQUAL = "=";
  std::string const BLOCK_START = "[";
  std::string const BLOCK_END = "];;"; 


  std::string name;
  std::string content;
  std::map<std::string, std::shared_ptr<ScsTree>> children;

  static std::shared_ptr<ScsTree> AddChildNode(
      std::shared_ptr<ScsTree> parent,
      std::string const & name,
      std::string const & content);
  static std::string Trim(std::string const & str);
  static bool IsBlockStart(std::string const & line);
  static bool IsBlockEnd(std::string const & line);
  static bool IsNodeWithContent(std::string const & line);
  static bool IsNodeWithoutContent(std::string const & line);
  static bool IsOpeningBracket(char ch);
  static bool IsClosingBracket(char ch);
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

bool ScsTree::IsBlockStart(std::string const & line)
{
  return line.find("=") != std::string::npos;
}

bool ScsTree::IsBlockEnd(std::string const & line)
{
  return line.find("*];;") != std::string::npos;
}

bool ScsTree::IsNodeWithContent(std::string const & line)
{
  return line.find(" = ") != std::string::npos;
}

bool ScsTree::IsNodeWithoutContent(std::string const & line)
{
  return !IsBlockStart(line) && !IsBlockEnd(line) && !IsNodeWithContent(line);
}

bool ScsTree::IsOpeningBracket(char ch)
{
  return ch == '[' || ch == '(';
}

bool ScsTree::IsClosingBracket(char ch)
{
  return ch == ']' || ch == ')';
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
  int bracketCount = 0;

  while (std::getline(iss, line))
  {
    line = Trim(line);
    if (line.empty())
      continue;

    if (isContentMultiLine)
    {
      multiLineContent += "\n" + line;
      for (char ch : line)
      {
        if (IsOpeningBracket(ch))
          bracketCount++;
        if (IsClosingBracket(ch))
          bracketCount--;
      }
      if (bracketCount == 0)
      {
        currentNode = AddChildNode(nodeStack.empty() ? root : nodeStack.back(), currentName, multiLineContent);
        isContentMultiLine = false;
        multiLineContent.clear();
      }
      continue;
    }

    if (IsBlockStart(line))
    {
      std::string name = Trim(line.substr(0, line.find(" = [*")));
      currentNode = AddChildNode(nodeStack.empty() ? root : nodeStack.back(), name, "");
      nodeStack.push_back(currentNode);
    }
    else if (IsBlockEnd(line))
    {
      if (!nodeStack.empty())
        nodeStack.pop_back();
      currentNode = nodeStack.empty() ? root : nodeStack.back();
    }
    else if (IsNodeWithContent(line))
    {
      size_t equalPos = line.find(" = ");
      std::string name = Trim(line.substr(0, equalPos));
      std::string content = Trim(line.substr(equalPos + 3));
      bracketCount = 0;
      for (char ch : content)
      {
        if (IsOpeningBracket(ch))
          bracketCount++;
        if (IsClosingBracket(ch))
          bracketCount--;
      }
      if (bracketCount != 0)
      {
        isContentMultiLine = true;
        multiLineContent = content;
        currentName = name;
      }
      else
      {
        currentNode = AddChildNode(nodeStack.empty() ? root : nodeStack.back(), name, content);
      }
    }
    else if (flag)
    {
      currentNode->content = line;
      flag = false;
    }
    else
    {
      std::string name = line;
      currentNode = AddChildNode(nodeStack.empty() ? root : nodeStack.back(), name, "");
      if (line.find(";;") == std::string::npos)
      {
        flag = true;
      }
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

void ScsTree::PrintDifferences(std::shared_ptr<std::vector<std::pair<std::string, std::string>>> const & differences)
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

std::shared_ptr<std::vector<std::pair<std::string, std::string>>> ScsTree::CompareTrees(
    std::shared_ptr<ScsTree> const & node1,
    std::shared_ptr<ScsTree> const & node2)
{
  auto differences = std::make_shared<std::vector<std::pair<std::string, std::string>>>();

  if (!node1 && !node2)
    return nullptr;
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
    differences->emplace_back("children size mismatch", "children size mismatch");
  }

  for (auto const & child : node1->children)
  {
    auto it = node2->children.find(child.first);
    if (it == node2->children.end())
    {
      differences->emplace_back(child.first, "missing in node2");
    }
    else
    {
      auto childDifferences = ScsTree::CompareTrees(child.second, it->second);
      if (childDifferences)
      {
        differences->insert(differences->end(), childDifferences->begin(), childDifferences->end());
      }
    }
  }

  for (auto const & child : node2->children)
  {
    if (node1->children.find(child.first) == node1->children.end())
    {
      differences->emplace_back("missing in node1", child.first);
    }
  }

  return differences->empty() ? nullptr : differences;
}
