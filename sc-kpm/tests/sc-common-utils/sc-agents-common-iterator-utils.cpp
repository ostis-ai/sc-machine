/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include "sc_test.hpp"
#include "dummy_file_interface.hpp"

#include "sc-memory/sc_scs_helper.hpp"

#include "sc-agents-common/utils/IteratorUtils.hpp"

TEST_F(ScMemoryTest, getNextFromSet)
{
  std::string const data =
      "@p1 = (set -> rrel_1: element_1);;"
      "@p2 = (set -> element_2);;"
      "@p1 => nrel_basic_sequence: @p2;;";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const & set = m_ctx->SearchElementBySystemIdentifier("set");
  EXPECT_TRUE(set.IsValid());
  ScAddr const & firstElement = m_ctx->SearchElementBySystemIdentifier("element_1");
  EXPECT_TRUE(firstElement.IsValid());
  ScAddr const & secondElement = m_ctx->SearchElementBySystemIdentifier("element_2");
  EXPECT_TRUE(secondElement.IsValid());

  ScAddr const & expectedSecondElement = utils::IteratorUtils::getNextFromSet(&*m_ctx, set, firstElement);
  EXPECT_TRUE(expectedSecondElement.IsValid());

  EXPECT_EQ(expectedSecondElement, secondElement);

  ScAddr const & emptyElement = utils::IteratorUtils::getNextFromSet(&*m_ctx, set, expectedSecondElement);
  EXPECT_FALSE(emptyElement.IsValid());
}

TEST_F(ScMemoryTest, getNextFromSetDouble)
{
  std::string const data =
      "@p1 = (set -> rrel_1: element_1);;"
      "@p2 = (set -> element_2);;"
      "@p3 = (set -> element_3);;"
      "@p1 => nrel_basic_sequence: @p2;;"
      "@p2 => nrel_basic_sequence: @p3;;";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const & set = m_ctx->SearchElementBySystemIdentifier("set");
  EXPECT_TRUE(set.IsValid());
  ScAddr const & firstElement = m_ctx->SearchElementBySystemIdentifier("element_1");
  EXPECT_TRUE(firstElement.IsValid());
  ScAddr const & secondElement = m_ctx->SearchElementBySystemIdentifier("element_2");
  EXPECT_TRUE(secondElement.IsValid());
  ScAddr const & thirdElement = m_ctx->SearchElementBySystemIdentifier("element_3");
  EXPECT_TRUE(thirdElement.IsValid());

  ScAddr const & expectedSecondElement = utils::IteratorUtils::getNextFromSet(&*m_ctx, set, firstElement);
  EXPECT_TRUE(expectedSecondElement.IsValid());
  EXPECT_EQ(expectedSecondElement, secondElement);

  ScAddr const & expectedThirdElement = utils::IteratorUtils::getNextFromSet(&*m_ctx, set, expectedSecondElement);
  EXPECT_TRUE(expectedThirdElement.IsValid());
  EXPECT_EQ(expectedThirdElement, thirdElement);

  ScAddr const & emptyElement = utils::IteratorUtils::getNextFromSet(&*m_ctx, set, expectedThirdElement);
  EXPECT_FALSE(emptyElement.IsValid());
}

TEST_F(ScMemoryTest, getNextFromSetNoSequence)
{
  std::string const data =
      "@p1 = (set -> rrel_1: element_1);;"
      "@p2 = (set -> element_2);;";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const & set = m_ctx->SearchElementBySystemIdentifier("set");
  EXPECT_TRUE(set.IsValid());
  ScAddr const & firstElement = m_ctx->SearchElementBySystemIdentifier("element_1");
  EXPECT_TRUE(firstElement.IsValid());

  ScAddr const & emptyElement = utils::IteratorUtils::getNextFromSet(&*m_ctx, set, firstElement);
  EXPECT_FALSE(emptyElement.IsValid());
}

TEST_F(ScMemoryTest, getAnyByOutRelation)
{
  std::string const data =
      "dialog"
      "  <- concept_dialog;"
      "  -> rrel_last: message; "
      "  -> rrel_1: message;; ";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const dialog = m_ctx->SearchElementBySystemIdentifier("dialog");
  EXPECT_TRUE(dialog.IsValid());
  ScAddr const relation = m_ctx->SearchElementBySystemIdentifier("rrel_1");
  EXPECT_TRUE(relation.IsValid());
  ScAddr const relationLast = m_ctx->SearchElementBySystemIdentifier("rrel_last");
  EXPECT_TRUE(relationLast.IsValid());

  ScAddr const message = utils::IteratorUtils::getAnyByOutRelation(&*m_ctx, dialog, relation);
  EXPECT_TRUE(message.IsValid());

  ScAddr const messageCopy = utils::IteratorUtils::getAnyByOutRelation(&*m_ctx, dialog, relationLast);
  EXPECT_TRUE(messageCopy.IsValid());

  EXPECT_EQ(message, messageCopy);
}
