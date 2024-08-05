#include "sc_test.hpp"

#include "sc-memory/sc_agent_context.hpp"

using ScAgentContextTest = ScMemoryTest;

TEST_F(ScAgentContextTest, MoveAgentContext)
{
  ScAgentContext context1;
  ScAgentContext context2 = ScAgentContext();
  EXPECT_TRUE(context2.IsValid());

  context1 = std::move(context2);

  EXPECT_TRUE(context1.IsValid());
  EXPECT_FALSE(context2.IsValid());

  context1 = std::move(context1);

  EXPECT_TRUE(context1.IsValid());
  EXPECT_FALSE(context2.IsValid());

  ScAgentContext context3 = std::move(context2);
  EXPECT_FALSE(context3.IsValid());
}
