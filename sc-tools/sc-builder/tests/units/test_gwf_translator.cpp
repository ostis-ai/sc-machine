/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder_test.hpp"

#include <sc-memory/sc_utils.hpp>

#include "gwf_translator.hpp"

#define BASE_TEST_PATH "/home/iromanchuk/sc-machine/sc-tools/sc-builder/tests/kb/tests-gwf-to-scs/"

using GWFTranslatorTest = ScBuilderTest;

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

struct TreeNode {
    std::string name;
    std::string content;
    std::map<std::string, std::shared_ptr<TreeNode>> children;

    TreeNode(const std::string& name, const std::string& content)
        : name(name), content(content) {}
};

std::shared_ptr<TreeNode> addChildNode(std::shared_ptr<TreeNode> parent, const std::string& name, const std::string& content) {
    auto node = std::make_shared<TreeNode>(name, content);
    if (parent) {
        parent->children.emplace(name, node);
    }
    return node;
}

std::string trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t");
    auto end = str.find_last_not_of(" \t");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

bool isBlockStart(const std::string& line) {
    return line.find(" = [*") != std::string::npos;
}

bool isBlockEnd(const std::string& line) {
    return line.find("*];;") != std::string::npos;
}

bool isNodeWithContent(const std::string& line) {
    return line.find(" = ") != std::string::npos;
}

bool isNodeWithoutContent(const std::string& line) {
    return !isBlockStart(line) && !isBlockEnd(line) && !isNodeWithContent(line);
}

bool isOpeningBracket(char ch) {
    return ch == '[' || ch == '(';
}

bool isClosingBracket(char ch) {
    return ch == ']' || ch == ')';
}

std::shared_ptr<TreeNode> parseTree(const std::string& input) {
    std::istringstream iss(input);
    std::string line;
    std::vector<std::shared_ptr<TreeNode>> nodeStack;
    auto root = std::make_shared<TreeNode>("root", "");
    auto currentNode = root;
    bool isContentMultiLine = false;
    bool flag = false;
    std::string multiLineContent;
    std::string currentName;
    int bracketCount = 0;

    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (isContentMultiLine) {
            multiLineContent += " " + line;
            for (char ch : line) {
                if (isOpeningBracket(ch)) bracketCount++;
                if (isClosingBracket(ch)) bracketCount--;
            }
            if (bracketCount == 0) {
                currentNode = addChildNode(nodeStack.empty() ? root : nodeStack.back(), currentName, multiLineContent);
                isContentMultiLine = false;
                multiLineContent.clear();
            }
            continue;
        }

        if (isBlockStart(line)) {
            std::string name = trim(line.substr(0, line.find(" = [*")));
            currentNode = addChildNode(nodeStack.empty() ? root : nodeStack.back(), name, "");
            nodeStack.push_back(currentNode);
        } else if (isBlockEnd(line)) {
            if (!nodeStack.empty()) nodeStack.pop_back();
            currentNode = nodeStack.empty() ? root : nodeStack.back();
        } else if (isNodeWithContent(line)) {
            size_t equalPos = line.find(" = ");
            std::string name = trim(line.substr(0, equalPos));
            std::string content = trim(line.substr(equalPos + 3));
            bracketCount = 0;
            for (char ch : content) {
                if (isOpeningBracket(ch)) bracketCount++;
                if (isClosingBracket(ch)) bracketCount--;
            }
            if (bracketCount != 0) {
                isContentMultiLine = true;
                multiLineContent = content;
                currentName = name;
            } else {
                currentNode = addChildNode(nodeStack.empty() ? root : nodeStack.back(), name, content);
            }
        }
        else if(flag){
            currentNode->content = line;
            flag = false;
        }
        else {
            std::string name = line;
            currentNode = addChildNode(nodeStack.empty() ? root : nodeStack.back(), name, "");
            flag = true;
        }
    }

    return root;
}

void printTree(const std::shared_ptr<TreeNode>& node, int level = 0) {
    if (!node) return;
    std::cout << std::string(level, '\t') << node->name << ": " << node->content << std::endl;
    for (const auto& child : node->children) {
        printTree(child.second, level + 1);
    }
}

bool compareTrees(const std::shared_ptr<TreeNode>& node1, const std::shared_ptr<TreeNode>& node2) {
    if (node1 == nullptr || node2 == nullptr) {
        return node1 == node2;
    }
    if (node1->name != node1->name || node1->content != node2->content) {
        return false;
    }
    if (node1->children.size() != node2->children.size()) {
        return false;
    }
    for (const auto& child : node1->children) {
        auto it = node2->children.find(child.first);
        if (it == node2->children.end() || !compareTrees(child.second, it->second)) {
            return false;
        }
    }
    return true;
}

TEST_F(GWFTranslatorTest, EmptyStaticSector)
{
  GWFTranslator translator(*m_ctx);

  const std::string filePath = BASE_TEST_PATH "empty_static.gwf";

  const std::string gwfStr = translator.XmlFileToString(filePath);

  EXPECT_THROW({ translator.GwfToScs(gwfStr, filePath); }, std::runtime_error);
}