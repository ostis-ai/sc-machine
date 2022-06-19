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

// TEST_F(ScServerTest, OneHundredConnections)
//{
//  size_t const connections = 100;
//  std::queue<std::unique_ptr<ScClient>> clients;
//
//  size_t i = 0;
//  while (i++ < connections)
//  {
//    auto client = std::make_unique<ScClient>();
//    EXPECT_TRUE(client->Connect(m_server->GetUri()));
//    client->Run();
//    clients.push(std::move(client));
//  }
//
//  sleep(5);
//
//  i = 0;
//  while (clients.empty() == SC_FALSE)
//  {
//    auto & client = clients.front();
//    client->Stop();
//    clients.pop();
//    ++i;
//  }
//}
