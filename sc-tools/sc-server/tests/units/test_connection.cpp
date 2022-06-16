#include <gtest/gtest.h>

#include "sc-memory/sc_link.hpp"

#include "sc_server_test.hpp"
#include "../sc_client.hpp"

TEST_F(ScServerTest, Connection)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();
  client.Stop();
}
