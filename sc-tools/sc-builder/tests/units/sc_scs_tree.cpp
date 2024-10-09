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

Differences SCsTreeNode::Compare(SCsTreeNode const & otherNode)
{
  auto differences = std::make_shared<std::list<std::pair<std::string, std::string>>>();

  for (auto const & child1 : this->children)
  {
    auto const & it = otherNode.children.find(child1.first);
    if (it == otherNode.children.cend())
      differences->emplace_back(child1.first + " with parent " + this->m_name, "missing");
    else
    {
      auto const & childDifferences = child1.second->Compare(*it->second);
      if (childDifferences)
        differences->splice(differences->cend(), *childDifferences);
    }
  }

  for (auto const & child2 : otherNode.children)
  {
    auto const & it = this->children.find(child2.first);
    if (it == this->children.cend())
      differences->emplace_back("missing", child2.first + " with parent " + otherNode.m_name);
  }

  return differences;
}

void SCsTree::Parse(std::string const & scsText)
{
  std::istringstream iss(scsText);
  std::string line;
  std::list<SCsTreeNodePtr> contoursStack;

  m_root = std::make_shared<SCsTreeNode>("root", "");

  bool isContentMultiLineInsideConnectorOrLink = false;
  bool isNodeMultiline = false;
  std::string multiLineContent;
  std::string currentName;

  while (std::getline(iss, line))
  {
    utils::StringUtils::Trim(line);
    if (line.empty())
      continue;

    auto parentNode = contoursStack.empty() ? m_root : contoursStack.back();

    if (isContentMultiLineInsideConnectorOrLink)
    {
      if (Check(BLOCK_END, line))
      {
        size_t const blockEndPos = line.find(BLOCK_END);
        multiLineContent += "\n" + line.substr(0, blockEndPos);

        parentNode->AddChildNode(currentName, multiLineContent);
        isContentMultiLineInsideConnectorOrLink = false;
        multiLineContent.clear();

        continue;
      }

      multiLineContent += "\n" + line;
      continue;
    }
    else if (isNodeMultiline)
    {
      size_t const elementEndPos = line.find(ELEMENT_END);

      std::string content = line.substr(0, elementEndPos);
      utils::StringUtils::Trim(content);

      parentNode->AddChildNode(currentName, content);

      isNodeMultiline = false;
      continue;
    }

    if (Check(EQUAL, line))
    {
      size_t const equalPos = line.find(EQUAL);

      std::string name = line.substr(0, equalPos);
      utils::StringUtils::Trim(name);

      if (Check(CONNECTOR_BEGIN, line))
      {
        size_t const connectorBeginPos = line.find(CONNECTOR_BEGIN) + CONNECTOR_BEGIN.length();

        if (Check(CONNECTOR_END, line))
        {
          size_t const connectorEndPos = line.find(CONNECTOR_END);

          std::string content = line.substr(connectorBeginPos, connectorEndPos - connectorBeginPos);
          utils::StringUtils::Trim(content);

          parentNode->AddChildNode(name, content);

          continue;
        }
        else
        {
          currentName = name;

          std::string content = line.substr(connectorBeginPos);
          utils::StringUtils::Trim(content);

          multiLineContent = content;
          isContentMultiLineInsideConnectorOrLink = true;
        }
      }
      else if (Check(CONTOUR_BEGIN, line))
      {
        auto const currentContour = parentNode->AddChildNode(name, "");
        contoursStack.push_back(currentContour);
      }
      else if (Check(BLOCK_BEGIN, line))
      {
        size_t const blockBeginPos = line.find(BLOCK_BEGIN) + BLOCK_BEGIN.length();

        if (Check(BLOCK_END, line))
        {
          size_t const blockEndPos = line.find(BLOCK_END);

          std::string content = line.substr(blockBeginPos, blockEndPos - blockBeginPos);
          utils::StringUtils::Trim(content);

          parentNode->AddChildNode(name, content);

          continue;
        }
        else
        {
          currentName = name;

          std::string content = line.substr(blockBeginPos);
          utils::StringUtils::Trim(content);

          multiLineContent = content;
          isContentMultiLineInsideConnectorOrLink = true;
        }
      }
    }
    else if (Check(CONTOUR_END, line))
      contoursStack.pop_back();
    else if (Check(ELEMENT_END, line))
    {
      size_t const elementEndPos = line.find(ELEMENT_END);

      std::string name = line.substr(0, elementEndPos);
      utils::StringUtils::Trim(name);

      parentNode->AddChildNode(name, "");
    }
    else
    {
      currentName = line;
      utils::StringUtils::Trim(currentName);

      // Use flag isNodeMultiline to correct process elements like:
      // ..el_102182951392320
      //  => nrel_main_idtf: [ostis];;

      isNodeMultiline = true;
    }
  }
}

Differences SCsTree::Compare(SCsTree const & otherTree)
{
  return this->m_root->Compare(*otherTree.m_root);
}

bool SCsTree::Check(std::string const & example, std::string const & line)
{
  return line.find(example) != std::string::npos;
}

std::string differencesToString(Differences const & differences)
{
  if (differences->empty())
    return "";
  std::ostringstream oss;
  oss << "Differences:\n";
  for (auto difference = differences->begin(); difference != differences->end(); ++difference)
  {
    if (difference != differences->begin())
    {
      oss << ",\n";
    }
    oss << "(" << difference->first << ", " << difference->second << ")";
  }
  return oss.str();
}
