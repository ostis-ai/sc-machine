#include <gtest/gtest.h>

#include "sc_test.hpp"
#include "dummy_file_interface.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"

#include "sc-agents-common/utils/IteratorUtils.hpp"

TEST_F(ScMemoryTest, getAnyByOutRelation)
{
  std::string const data = "dialog"
                           "  <- concept_dialog;"
                           "  -> rrel_last: message; "
                           "  -> rrel_1: message;; ";

  SCsHelper helper(*m_ctx, std::make_shared<DummyFileInterface>());
  EXPECT_TRUE(helper.GenerateBySCsText(data));

  ScAddr const dialog = m_ctx->HelperFindBySystemIdtf("dialog");
  EXPECT_TRUE(dialog.IsValid());
  ScAddr const relation = m_ctx->HelperFindBySystemIdtf("rrel_1");
  EXPECT_TRUE(relation.IsValid());
  ScAddr const relationLast = m_ctx->HelperFindBySystemIdtf("rrel_last");
  EXPECT_TRUE(relationLast.IsValid());

  ScAddr const message = utils::IteratorUtils::getAnyByOutRelation(&*m_ctx, dialog, relation);
  EXPECT_TRUE(message.IsValid());

  ScAddr const messageCopy = utils::IteratorUtils::getAnyByOutRelation(&*m_ctx, dialog, relationLast);
  EXPECT_TRUE(messageCopy.IsValid());

  EXPECT_TRUE(message == messageCopy);
}

