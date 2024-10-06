/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <filesystem>

#include <sc-memory/sc_utils.hpp>

#include "builder_test.hpp"

#include "gwf_translator.hpp"

#include "sc_scs_tree.hpp"

using GWFTranslatorTest = ScBuilderTest;

#define BASE_TEST_PATH SC_BUILDER_KB "/tests-gwf-to-scs/"

std::vector<char> ReadFileToBytes(std::string const & filePath)
{
  std::ifstream file(filePath, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size);
  file.read(buffer.data(), size);
  return buffer;
}

bool CompareFiles(std::string const & filePath1, std::string const & filePath2)
{
  std::vector<char> file1Bytes = ReadFileToBytes(filePath1);
  std::vector<char> file2Bytes = ReadFileToBytes(filePath2);
  return file1Bytes == file2Bytes;
}

std::string ReadFileToString(std::string const & filePath)
{
  std::ifstream file(filePath);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

bool CheckGWFToSCSTranslation(std::unique_ptr<ScAgentContext> & context, std::string const & fileName)
{
  GWFTranslator translator(*context);

  std::string const & testGWFFilePath = BASE_TEST_PATH + fileName;
  std::string const & correctSCsFilePath = testGWFFilePath + ".scs";

  std::string const & testGWFText = translator.GetXMLFileContent(testGWFFilePath);
  std::string const & testSCsText = translator.TranslateGWFToSCs(testGWFText, BASE_TEST_PATH);

  std::string const & correctSCsText = ReadFileToString(correctSCsFilePath);

  SCsTree testSCsTree;
  testSCsTree.Parse(testSCsText);
  SCsTree correctSCsTree;
  correctSCsTree.Parse(correctSCsText);
  return testSCsTree.Compare(correctSCsTree)->empty();
}

TEST_F(GWFTranslatorTest, InvalidPath)
{
  GWFTranslator translator(*m_ctx);

  std::string const filePath = BASE_TEST_PATH "invalid_path.gwf";
  EXPECT_THROW(translator.GetXMLFileContent(filePath), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, EmptyFile)
{
  GWFTranslator translator(*m_ctx);

  std::string const filePath = BASE_TEST_PATH "empty_file.gwf";
  EXPECT_THROW(translator.GetXMLFileContent(filePath), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, EmptyStatic)
{
  GWFTranslator translator(*m_ctx);

  std::string const & filePath = BASE_TEST_PATH "empty_static.gwf";
  std::string const & gwfText = translator.GetXMLFileContent(filePath);
  EXPECT_THROW(translator.TranslateGWFToSCs(gwfText, BASE_TEST_PATH), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, EmptyGWF)
{
  GWFTranslator translator(*m_ctx);
  EXPECT_THROW(translator.TranslateGWFToSCs("", BASE_TEST_PATH), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, UnknownTag)
{
  GWFTranslator translator(*m_ctx);

  std::string const filePath = BASE_TEST_PATH "unknown_tag.gwf";
  std::string const & gwfText = translator.GetXMLFileContent(filePath);
  EXPECT_THROW(translator.TranslateGWFToSCs(gwfText, BASE_TEST_PATH), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, UnknownLinkContent)
{
  GWFTranslator translator(*m_ctx);

  std::string const filePath = BASE_TEST_PATH "unknown_link_content.gwf";
  std::string const & gwfText = translator.GetXMLFileContent(filePath);
  EXPECT_THROW(translator.TranslateGWFToSCs(gwfText, BASE_TEST_PATH), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, ContourWithUnknownParent)
{
  GWFTranslator translator(*m_ctx);

  std::string const filePath = BASE_TEST_PATH "contour_with_unknown_parent.gwf";
  std::string const & gwfText = translator.GetXMLFileContent(filePath);
  EXPECT_THROW(translator.TranslateGWFToSCs(gwfText, BASE_TEST_PATH), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, NodeWithUnknownId)
{
  GWFTranslator translator(*m_ctx);

  std::string const filePath = BASE_TEST_PATH "node_with_unknown_id.gwf";
  std::string const & gwfText = translator.GetXMLFileContent(filePath);
  EXPECT_THROW(translator.TranslateGWFToSCs(gwfText, BASE_TEST_PATH), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, NodeWithUnknownType)
{
  GWFTranslator translator(*m_ctx);

  std::string const filePath = BASE_TEST_PATH "element_with_unknown_type.gwf";
  std::string const & gwfText = translator.GetXMLFileContent(filePath);
  EXPECT_THROW(translator.TranslateGWFToSCs(gwfText, BASE_TEST_PATH), utils::ExceptionItemNotFound);
}

TEST_F(GWFTranslatorTest, EmptyContour)
{
  EXPECT_TRUE(CheckGWFToSCSTranslation(m_ctx, "empty_contour.gwf"));
}

TEST_F(GWFTranslatorTest, LotOfContours)
{
  EXPECT_TRUE(CheckGWFToSCSTranslation(m_ctx, "lot_of_contours.gwf"));
}

TEST_F(GWFTranslatorTest, ContentTypes)
{
  EXPECT_TRUE(CheckGWFToSCSTranslation(m_ctx, "content_types.gwf"));

  EXPECT_TRUE(CompareFiles(BASE_TEST_PATH "ostis.png", BASE_TEST_PATH "ostis_ref.png"));
  std::filesystem::remove(BASE_TEST_PATH "ostis.png");
}

TEST_F(GWFTranslatorTest, SCsTree)
{
  GWFTranslator translator(*m_ctx);

  std::string const & testGWFFilePath = BASE_TEST_PATH "empty_contour.gwf";
  std::string const & correctGWFFilePath = BASE_TEST_PATH "lot_of_contours.gwf";
  std::string const & correctSCsFilePath = correctGWFFilePath + ".scs";

  std::string const & testGWFText = translator.GetXMLFileContent(testGWFFilePath);
  std::string const & testSCsText = translator.TranslateGWFToSCs(testGWFText, BASE_TEST_PATH);

  std::string const & correctSCsText = ReadFileToString(correctSCsFilePath);

  SCsTree testSCsTree;
  testSCsTree.Parse(testSCsText);
  SCsTree correctSCsTree;
  correctSCsTree.Parse(correctSCsText);
  EXPECT_FALSE(testSCsTree.Compare(correctSCsTree)->empty());
  EXPECT_FALSE(correctSCsTree.Compare(testSCsTree)->empty());
}
