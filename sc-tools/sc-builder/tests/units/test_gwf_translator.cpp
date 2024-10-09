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
#include <memory>
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

Differences CheckGWFToSCSTranslation(std::string const & fileName)
{
  std::string const & testGWFFilePath = BASE_TEST_PATH + fileName;
  std::string const & correctSCsFilePath = testGWFFilePath + ".scs";

  std::string const & testSCsText = GWFTranslator::TranslateXMLFileContentToSCs(testGWFFilePath);

  std::string const & correctSCsText = ReadFileToString(correctSCsFilePath);

  SCsTree testSCsTree;
  testSCsTree.Parse(testSCsText);
  SCsTree correctSCsTree;
  correctSCsTree.Parse(correctSCsText);
  return testSCsTree.Compare(correctSCsTree);
}

TEST_F(GWFTranslatorTest, InvalidPath)
{
  GWFTranslator translator(*m_ctx);

  std::string const filePath = BASE_TEST_PATH "invalid_path.gwf";
  EXPECT_THROW(translator.TranslateXMLFileContentToSCs(filePath), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, EmptyFile)
{
  GWFTranslator translator(*m_ctx);

  std::string const filePath = BASE_TEST_PATH "empty_file.gwf";
  EXPECT_THROW(translator.TranslateXMLFileContentToSCs(filePath), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, EmptyStatic)
{
  std::string const & filePath = BASE_TEST_PATH "empty_static.gwf";
  EXPECT_THROW(GWFTranslator::TranslateXMLFileContentToSCs(filePath), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, NoStatic)
{
  std::string const & filePath = BASE_TEST_PATH "no_static_sector.gwf";
  EXPECT_THROW(GWFTranslator::TranslateXMLFileContentToSCs(filePath), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, EmptyGWF)
{
  EXPECT_THROW(GWFTranslator::TranslateXMLFileContentToSCs(BASE_TEST_PATH), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, UnknownTag)
{
  std::string const filePath = BASE_TEST_PATH "unknown_tag.gwf";
  EXPECT_THROW(GWFTranslator::TranslateXMLFileContentToSCs(filePath), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, UnknownLinkContent)
{
  std::string const filePath = BASE_TEST_PATH "unknown_link_content.gwf";
  EXPECT_THROW(GWFTranslator::TranslateXMLFileContentToSCs(filePath), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, ContourWithUnknownParent)
{
  std::string const filePath = BASE_TEST_PATH "contour_with_unknown_parent.gwf";
  EXPECT_THROW(GWFTranslator::TranslateXMLFileContentToSCs(filePath), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, NodeWithUnknownId)
{
  std::string const filePath = BASE_TEST_PATH "node_with_unknown_id.gwf";
  EXPECT_THROW(GWFTranslator::TranslateXMLFileContentToSCs(filePath), utils::ExceptionParseError);
}

TEST_F(GWFTranslatorTest, NodeWithUnknownType)
{
  std::string const filePath = BASE_TEST_PATH "element_with_unknown_type.gwf";
  EXPECT_THROW(GWFTranslator::TranslateXMLFileContentToSCs(filePath), utils::ExceptionItemNotFound);
}

TEST_F(GWFTranslatorTest, ConnectorLoopedOnItself)
{
  std::string const filePath = BASE_TEST_PATH "connector_from_itself_to_itself.gwf";
  EXPECT_THROW(GWFTranslator::TranslateXMLFileContentToSCs(filePath), utils::ExceptionInvalidState);
}

TEST_F(GWFTranslatorTest, Bus)
{
  Differences const & differences = CheckGWFToSCSTranslation("bus.gwf");
  EXPECT_TRUE(differences->empty()) << differencesToString(differences);
}

TEST_F(GWFTranslatorTest, EmptyContour)
{
  Differences const & differences = CheckGWFToSCSTranslation("empty_contour.gwf");
  EXPECT_TRUE(differences->empty()) << differencesToString(differences);
}

TEST_F(GWFTranslatorTest, LotOfContours)
{
  Differences const & differences = CheckGWFToSCSTranslation("lot_of_contours.gwf");
  EXPECT_TRUE(differences->empty()) << differencesToString(differences);
}

TEST_F(GWFTranslatorTest, ContourWithMainKeyScElement)
{
  Differences const & differences = CheckGWFToSCSTranslation("contour_with_main_key_sc_element.gwf");
  EXPECT_TRUE(differences->empty()) << differencesToString(differences);
}

TEST_F(GWFTranslatorTest, ContentTypes)
{
  Differences const & differences = CheckGWFToSCSTranslation("content_types.gwf");
  EXPECT_TRUE(differences->empty()) << differencesToString(differences);

  EXPECT_TRUE(CompareFiles(BASE_TEST_PATH "ostis.png", BASE_TEST_PATH "ostis_ref.png"));
  std::filesystem::remove(BASE_TEST_PATH "ostis.png");
}

TEST_F(GWFTranslatorTest, SCsTree)
{
  std::string const & emptyContourGWFFilePath = BASE_TEST_PATH "empty_contour.gwf";
  std::string const & lotOfContoursGWFFilePath = BASE_TEST_PATH "lot_of_contours.gwf";
  std::string const & lotOfContoursSCsFilePath = lotOfContoursGWFFilePath + ".scs";

  std::string const & emptyContourSCsText = GWFTranslator::TranslateXMLFileContentToSCs(emptyContourGWFFilePath);

  std::string const & lotOfContoursSCsText = ReadFileToString(lotOfContoursSCsFilePath);

  SCsTree emptyContourSCsTree;
  emptyContourSCsTree.Parse(emptyContourSCsText);
  SCsTree lotOfContoursSCsTree;
  lotOfContoursSCsTree.Parse(lotOfContoursSCsText);
  Differences const & emptyCompared = emptyContourSCsTree.Compare(lotOfContoursSCsTree);
  EXPECT_FALSE(emptyCompared->empty());
  Differences const & notEmptyCompared = lotOfContoursSCsTree.Compare(emptyContourSCsTree);
  EXPECT_FALSE(notEmptyCompared->empty());
  for (auto const & differenceWithEmptyFirst : *emptyCompared)
  {
    EXPECT_NE(
        std::find(
            notEmptyCompared->cbegin(),
            notEmptyCompared->cend(),
            std::make_pair(differenceWithEmptyFirst.second, differenceWithEmptyFirst.first)),
        notEmptyCompared->cend())
        << "not found pair " << differenceWithEmptyFirst.first << ", " << differenceWithEmptyFirst.second;
  }
  for (auto const & differenceWithNotEmptyFirst : *notEmptyCompared)
  {
    EXPECT_NE(
        std::find(
            emptyCompared->cbegin(),
            emptyCompared->cend(),
            std::make_pair(differenceWithNotEmptyFirst.second, differenceWithNotEmptyFirst.first)),
        notEmptyCompared->cend())
        << "not found pair " << differenceWithNotEmptyFirst.first << ", " << differenceWithNotEmptyFirst.second;
  }
}

class TestGWFTranslator : public GWFTranslator
{
public:
  explicit TestGWFTranslator(class ScMemoryContext & context)
    : GWFTranslator(context)
  {
  }

  using GWFTranslator::WriteStringToFile;
};

TEST_F(GWFTranslatorTest, WriteStringToFileInUnknownDirectory)
{
  TestGWFTranslator translator(*m_ctx);
  EXPECT_THROW(translator.WriteStringToFile("text", "test/test.txt"), utils::ExceptionCritical);
}

TEST_F(GWFTranslatorTest, DifferencesToString)
{
  Differences differences = std::make_shared<std::list<std::pair<std::string, std::string>>>();
  differences->emplace_back("first", "second");
  differences->emplace_back("third", "fourth");
  std::string expectedDifferences =
      "Differences:\n"
      "(first, second),\n"
      "(third, fourth)";
  EXPECT_EQ(differencesToString(differences), expectedDifferences);
}
