/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder_test.hpp"

#include <sc-memory/sc_utils.hpp>

#include "gwf_translator.hpp"

#define TEST_CORRECT_GWF SC_BUILDER_KB "/test-templates/test_template_1.gwf"
#define TEST_INCORRECT_GWF SC_BUILDER_TEST_REPOS "/kb/example.gwf"

TEST_F(ScBuilderTest, CorrectGWF)
{
  GWFTranslator translator(*m_ctx);

  Translator::Params translateParams;
  translateParams.m_fileName = TEST_CORRECT_GWF;
  translateParams.m_autoFormatInfo = true;
  translateParams.m_outputStructure = ScAddr::Empty;
  EXPECT_TRUE(translator.Translate(translateParams));
}

TEST_F(ScBuilderTest, IncorrectGWF)
{
  GWFTranslator translator(*m_ctx);

  Translator::Params translateParams;
  translateParams.m_fileName = TEST_INCORRECT_GWF;
  translateParams.m_autoFormatInfo = true;
  translateParams.m_outputStructure = ScAddr::Empty;
  EXPECT_THROW(translator.Translate(translateParams), utils::ExceptionParseError);
}
