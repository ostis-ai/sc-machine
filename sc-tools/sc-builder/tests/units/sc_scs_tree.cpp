/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_scs_tree.hpp"

using namespace Constants;

SCsTreeNode::SCsTreeNode(std::string const & name, std::string const & content)
  : m_name(name)
  , m_content(content)
{
}

SCsTreeNodePtr SCsTreeNode::AddChildNode(std::string const & name, std::string const & content)
{
  auto node = std::make_shared<SCsTreeNode>(name, content);
  this->children.emplace(name, node);
  return node;
}

std::shared_ptr<std::list<std::pair<std::string, std::string>>> SCsTreeNode::Compare(SCsTreeNode const & otherNode)
{
  auto differences = std::make_shared<std::list<std::pair<std::string, std::string>>>();

  for (auto const & child1 : this->children)
  {
    auto it = otherNode.children.find(child1.first);
    if (it == otherNode.children.cend())
      differences->emplace_back(child1.first, "missing");
    else
    {
      auto childDifferences = child1.second->Compare(*it->second);
      if (childDifferences)
        differences->insert(differences->cend(), childDifferences->cbegin(), childDifferences->cend());
    }
  }

  for (auto const & child2 : otherNode.children)
  {
    if (this->children.find(child2.first) == this->children.cend())
      differences->emplace_back("missing", child2.first);
  }

  return differences;
}

void SCsTree::Parse(std::string const & scsText)
{
  std::istringstream iss(scsText);
  std::string line;
  std::list<SCsTreeNodePtr> nodeStack;

  m_root = std::make_shared<SCsTreeNode>("root", "");

  auto currentNode = m_root;
  bool isContentMultiLine = false;
  bool flag = false;
  std::string multiLineContent;
  std::string currentName;

  while (std::getline(iss, line))
  {
    utils::StringUtils::Trim(line);
    if (line.empty())
      continue;

    auto parentNode = nodeStack.empty() ? m_root : nodeStack.back();

    if (isContentMultiLine)
    {
      if (Check(BLOCK_END, line))
      {
        size_t blockEndPos = line.find(BLOCK_END);
        multiLineContent += "\n" + line.substr(0, blockEndPos);

        currentNode = parentNode->AddChildNode(currentName, multiLineContent);
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

      std::string content = line.substr(0, elementEndPos);
      utils::StringUtils::Trim(content);

      currentNode = parentNode->AddChildNode(currentName, content);

      flag = false;
      continue;
    }

    if (Check(EQUAL, line))
    {
      size_t equalPos = line.find(EQUAL);

      std::string name = line.substr(0, equalPos);
      utils::StringUtils::Trim(name);

      if (Check(CONNECTOR_BEGIN, line))
      {
        size_t edgeBeginPos = line.find(CONNECTOR_BEGIN) + CONNECTOR_BEGIN.length();

        if (Check(CONNECTOR_END, line))
        {
          size_t edgeEndPos = line.find(CONNECTOR_END);

          std::string content = line.substr(edgeBeginPos, edgeEndPos - edgeBeginPos);
          utils::StringUtils::Trim(content);

          currentNode = parentNode->AddChildNode(name, content);

          continue;
        }
        else
        {
          currentName = name;

          std::string content = line.substr(edgeBeginPos);
          utils::StringUtils::Trim(content);

          multiLineContent = content;
          isContentMultiLine = true;
        }
      }
      else if (Check(CONTOUR_BEGIN, line))
      {
        currentNode = parentNode->AddChildNode(name, "");
        nodeStack.push_back(currentNode);
      }
      else if (Check(BLOCK_BEGIN, line))
      {
        size_t blockBeginPos = line.find(BLOCK_BEGIN) + BLOCK_BEGIN.length();

        if (Check(BLOCK_END, line))
        {
          size_t blockEndPos = line.find(BLOCK_END);

          std::string content = line.substr(blockBeginPos, blockEndPos - blockBeginPos);
          utils::StringUtils::Trim(content);

          currentNode = parentNode->AddChildNode(name, content);

          continue;
        }
        else
        {
          currentName = name;

          std::string content = line.substr(blockBeginPos);
          utils::StringUtils::Trim(content);

          multiLineContent = content;
          isContentMultiLine = true;
        }
      }
    }
    else if (Check(CONTOUR_END, line))
    {
      nodeStack.pop_back();
      currentNode = parentNode;
    }
    else if (Check(ELEMENT_END, line))
    {
      size_t elementEndPos = line.find(ELEMENT_END);

      std::string name = line.substr(0, elementEndPos);
      utils::StringUtils::Trim(name);

      currentNode = parentNode->AddChildNode(name, "");
    }
    else
    {
      currentName = line;
      utils::StringUtils::Trim(currentName);

      // Use flag to correct process elements like:
      // ..el_102182951392320
      //  => nrel_main_idtf: [ostis];;

      flag = true;
    }
  }
}

std::shared_ptr<std::list<std::pair<std::string, std::string>>> SCsTree::Compare(SCsTree const & otherTree)
{
  return this->m_root->Compare(*otherTree.m_root);
}

bool SCsTree::Check(std::string const & example, std::string const & line)
{
  return line.find(example) != std::string::npos;
}
