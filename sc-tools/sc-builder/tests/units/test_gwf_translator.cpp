/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder_test.hpp"

#include <sc-memory/sc_utils.hpp>

#include "sc-memory/sc_utils.hpp"
#include "../../src/gwf_translator.hpp"
#include "../../src/sc_scs_tree.hpp"

#define BASE_TEST_PATH "/home/iromanchuk/sc-machine/sc-tools/sc-builder/tests/kb/tests-gwf-to-scs/"

using GWFTranslatorTest = ScBuilderTest;

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

std::vector<char> readFileToBytes(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) {
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Could not open file: " + filePath);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Could not read file: " + filePath);
    }
    return buffer;
}

bool compareFiles(const std::string& filePath1, const std::string& filePath2) {
    try {
        std::vector<char> file1Bytes = readFileToBytes(filePath1);
        std::vector<char> file2Bytes = readFileToBytes(filePath2);
        return file1Bytes == file2Bytes;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

std::string readFileToString(std::string const & filePath)
{
  std::ifstream file(filePath);
  if (!file.is_open())
  {
    throw std::runtime_error("Could not open file");
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

TEST_F(GWFTranslatorTest, EmptyContour)
{
  GWFTranslator translator(*m_ctx);

  const std::string filePath = BASE_TEST_PATH "empty_contour.gwf";

  const std::string gwfStr = translator.XmlFileToString(filePath);
  const std::string scsStr = translator.GwfToScs(gwfStr, BASE_TEST_PATH);

  const std::string exampleScs = readFileToString(BASE_TEST_PATH "empty_contour.gwf.scs");

  auto const exampleTree = ScsTree::ParseTree(exampleScs);
  auto const resultTree = ScsTree::ParseTree(scsStr);

  EXPECT_FALSE(ScsTree::CompareTrees(exampleTree, resultTree));
}

TEST_F(GWFTranslatorTest, LotOfContours)
{
  GWFTranslator translator(*m_ctx);

  const std::string filePath = BASE_TEST_PATH "lot_of_contours.gwf";

  const std::string gwfStr = translator.XmlFileToString(filePath);
  const std::string scsStr = translator.GwfToScs(gwfStr, BASE_TEST_PATH);

  const std::string exampleScs = readFileToString(BASE_TEST_PATH "lot_of_contours.gwf.scs");

  auto const exampleTree = ScsTree::ParseTree(exampleScs);
  auto const resultTree = ScsTree::ParseTree(scsStr);

  EXPECT_FALSE(ScsTree::CompareTrees(exampleTree, resultTree));
}

TEST_F(GWFTranslatorTest, ContentTypes)
{
  GWFTranslator translator(*m_ctx);

  const std::string filePath = BASE_TEST_PATH "content_types.gwf";

  const std::string gwfStr = translator.XmlFileToString(filePath);
  const std::string scsStr = translator.GwfToScs(gwfStr, BASE_TEST_PATH);

  const std::string exampleScs = readFileToString(BASE_TEST_PATH "content_types.gwf.scs");

  auto const exampleTree = ScsTree::ParseTree(exampleScs);
  auto const resultTree = ScsTree::ParseTree(scsStr);

  EXPECT_FALSE(ScsTree::CompareTrees(exampleTree, resultTree));
}