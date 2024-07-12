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

bool compareStringWithFileContent(std::string const & str, std::string const & filePath)
{
  std::ifstream file(filePath);
  if (!file.is_open())
  {
    std::cerr << "Failed to open file: " << filePath << std::endl;
    return false;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string fileContent = buffer.str();

  return str == fileContent;
}

std::vector<char> readFile(std::string const & filePath)
{
  std::ifstream file(filePath, std::ios::binary);
  if (!file)
  {
    std::cerr << "Could not open the file " << filePath << std::endl;
    return {};
  }

  return std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

bool isFileIdentical(std::string const & filePath1, std::string const & filePath2)
{
  std::vector<char> fileData1 = readFile(filePath1);
  std::vector<char> fileData2 = readFile(filePath2);

  if (fileData1.empty() || fileData2.empty())
  {
    return false;
  }

  if (fileData1.size() != fileData2.size())
  {
    return false;
  }

  return std::equal(fileData1.begin(), fileData1.end(), fileData2.begin());
}

TEST_F(GWFTranslatorTest, EmptyStaticSector)
{
  GWFTranslator translator(*m_ctx);

  const std::string filePath = BASE_TEST_PATH "empty_static.gwf";

  const std::string gwfStr = translator.XmlFileToString(filePath);

  EXPECT_THROW({ translator.GwfToScs(gwfStr, filePath); }, std::runtime_error);
}

TEST_F(GWFTranslatorTest, EmptyFile)
{
  GWFTranslator translator(*m_ctx);

  const std::string filePath = BASE_TEST_PATH "empty.gwf";

  EXPECT_THROW({ translator.XmlFileToString(filePath); }, std::runtime_error);
}

TEST_F(GWFTranslatorTest, ContentTypes)
{
  GWFTranslator translator(*m_ctx);

  const std::string filePath = BASE_TEST_PATH "content_types.gwf";

  const std::string gwfStr = translator.XmlFileToString(filePath);

  const std::string scsStr = translator.GwfToScs(gwfStr, filePath);

  EXPECT_TRUE(compareStringWithFileContent(scsStr, filePath + ".scs"));
  EXPECT_TRUE(isFileIdentical(BASE_TEST_PATH "ostis.png", BASE_TEST_PATH "ostis_ref.png"));

  std::filesystem::remove(BASE_TEST_PATH "ostis.png");
}

TEST_F(GWFTranslatorTest, LotOfContours)
{
  GWFTranslator translator(*m_ctx);

  const std::string filePath = BASE_TEST_PATH "lot_of_contours.gwf";

  const std::string gwfStr = translator.XmlFileToString(filePath);

  const std::string scsStr = translator.GwfToScs(gwfStr, filePath);

  EXPECT_TRUE(compareStringWithFileContent(scsStr, filePath + ".scs"));
}

TEST_F(GWFTranslatorTest, EmptyContour)
{
  GWFTranslator translator(*m_ctx);

  const std::string filePath = BASE_TEST_PATH "empty_contour.gwf";

  const std::string gwfStr = translator.XmlFileToString(filePath);

  const std::string scsStr = translator.GwfToScs(gwfStr, filePath);

  EXPECT_TRUE(compareStringWithFileContent(scsStr, filePath + ".scs"));
}