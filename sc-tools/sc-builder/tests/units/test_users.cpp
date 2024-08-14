/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include <thread>

#include <sc-memory/sc_event_subscription.hpp>

#include "sc-core/sc_keynodes.h"

#include "builder_test.hpp"

#define SC_LOCK_WAIT_WHILE_TRUE(expression) \
  ({ \
    sc_uint32 retries = 50; \
    sc_uint32 i = 0; \
    while (expression && i < retries) \
    { \
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); \
      ++i; \
    } \
  })

void TestAuthenticationRequestUser(
    std::unique_ptr<ScAgentContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & conceptAuthenticationRequestUserAddr{concept_authentication_request_user_addr};
  context->CreateEdge(arcType, conceptAuthenticationRequestUserAddr, userAddr);
}

TEST_F(ScBuilderLoadUserPermissionsTest, UserWithGlobalReadPermissionsAndWithLocalWritePermissions)
{
  ScAddr const & userAddr = m_ctx->HelperFindBySystemIdtf("test_user_1");

  TestScMemoryContext userContext{userAddr};
  EXPECT_THROW(userContext.CreateNode(ScType::NodeConst), utils::ExceptionInvalidState);

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription = m_ctx->CreateEventSubscription<ScEventAddOutputArc<ScType::EdgeAccess>>(
      conceptAuthenticatedUserAddr,
      [&userContext, &isAuthenticated](ScEventAddOutputArc<ScType::EdgeAccess> const &)
      {
        ScAddr const & otherUserAddr = userContext.HelperFindBySystemIdtf("test_user_2");
        ScAddr const & classAddr = userContext.CreateNode(ScType::NodeConstClass);
        EXPECT_THROW(
            userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, classAddr, otherUserAddr),
            utils::ExceptionInvalidState);
        ScAddr const & userStructureAddr = userContext.HelperFindBySystemIdtf("user_structure");
        EXPECT_THROW(userContext.EraseElement(userStructureAddr), utils::ExceptionInvalidState);
        EXPECT_NO_THROW(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, userStructureAddr, classAddr));

        isAuthenticated = true;
      });

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScBuilderLoadUserPermissionsTest, UserWithGlobalReadPermissionsAndWithoutLocalWritePermissions)
{
  ScAddr const & userAddr = m_ctx->HelperFindBySystemIdtf("test_user_2");

  TestScMemoryContext userContext{userAddr};
  EXPECT_THROW(userContext.CreateNode(ScType::NodeConst), utils::ExceptionInvalidState);

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  auto eventSubscription = m_ctx->CreateEventSubscription<ScEventAddOutputArc<ScType::EdgeAccess>>(
      conceptAuthenticatedUserAddr,
      [&userContext, &isAuthenticated](ScEventAddOutputArc<ScType::EdgeAccess> const &)
      {
        ScAddr const & otherUserAddr = userContext.HelperFindBySystemIdtf("test_user_2");
        ScAddr const & classAddr = userContext.CreateNode(ScType::NodeConstClass);
        EXPECT_THROW(
            userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, classAddr, otherUserAddr),
            utils::ExceptionInvalidState);
        ScAddr const & userStructureAddr = userContext.HelperFindBySystemIdtf("user_structure");
        EXPECT_THROW(userContext.EraseElement(userStructureAddr), utils::ExceptionInvalidState);
        EXPECT_THROW(
            userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, userStructureAddr, classAddr),
            utils::ExceptionInvalidState);

        isAuthenticated = true;
      });

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}