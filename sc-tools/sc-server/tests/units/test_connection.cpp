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

void TEST_N_CONNECTIONS(std::unique_ptr<ScServer> const & server, size_t const amount)
{
  size_t const CONNECTIONS = amount;
  std::queue<std::unique_ptr<ScClient>> clients;

  auto start = std::chrono::high_resolution_clock::now();

  size_t const WAIT_BETWEEN_CLIENTS_CONNECT = 10;
  size_t i = 0;
  while (i++ < CONNECTIONS)
  {
    auto client = std::make_unique<ScClient>();
    EXPECT_TRUE(client->Connect(server->GetUri()));
    client->Run();
    clients.push(std::move(client));

    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_BETWEEN_CLIENTS_CONNECT));
  }

  size_t const WAIT_BETWEEN_CONNECT_DISCONNECT = 5000;
  std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_BETWEEN_CONNECT_DISCONNECT));

  i = 0;
  while (clients.empty() == SC_FALSE)
  {
    auto & client = clients.front();
    client->Stop();
    clients.pop();

    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_BETWEEN_CLIENTS_CONNECT));
    ++i;
  }

  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double, std::milli> elapsed = end - start;
  std::cout << "Elapsed "
            << elapsed.count() - WAIT_BETWEEN_CONNECT_DISCONNECT -
                   2. * (double)CONNECTIONS * WAIT_BETWEEN_CLIENTS_CONNECT
            << " ms\n";
}

TEST_F(ScServerTest, DISABLED_OneHundredConnections)  // 206.55 ms
{
  TEST_N_CONNECTIONS(m_server, 100);
}

TEST_F(ScServerTest, DISABLED_OneThousandConnections)  // 2583.39 ms
{
  TEST_N_CONNECTIONS(m_server, 1000);
}
