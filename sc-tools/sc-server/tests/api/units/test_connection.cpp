/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include "sc_server_test.hpp"
#include "../../sc_client.hpp"
#include "../../../src/sc_server_setup.hpp"

#include "sc_options.hpp"
#include "sc-config/sc_config.hpp"
#include "sc_memory_config.hpp"

TEST(ScServer, RunMain)
{
  sc_uint32 const argsNumber = 4;
  sc_char const * args[argsNumber] = {"sc-server", "-c", SC_SERVER_INI, "-t"};
  EXPECT_EQ(BuildAndRunServer(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScServer, RunMainWithExtensions)
{
  sc_uint32 const argsNumber = 6;
  sc_char const * args[argsNumber] = {"sc-server", "-c", SC_SERVER_INI, "-e", SC_SERVER_EXTENSIONS, "-t"};
  EXPECT_EQ(BuildAndRunServer(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScServer, InvalidRunMain)
{
  sc_uint32 const argsNumber = 1;
  sc_char const * args[argsNumber] = {"sc-server"};
  EXPECT_EQ(BuildAndRunServer(argsNumber, (sc_char **)args), EXIT_FAILURE);
}

TEST(ScServer, RunMainHelp)
{
  sc_uint32 const argsNumber = 2;
  sc_char const * args[argsNumber] = {"sc-server", "--help"};
  EXPECT_EQ(BuildAndRunServer(argsNumber, (sc_char **)args), EXIT_SUCCESS);
}

TEST(ScServer, RunStopServer)
{
  ScOptions options{1, nullptr};

  std::string configFile = SC_SERVER_INI;

  ScParams serverParams{options, {}};

  ScConfig config{configFile, {}};
  auto serverConfig = config["sc-server"];
  for (auto const & key : *serverConfig)
    serverParams.Insert({key, serverConfig[key]});

  ScParams memoryParams{options, {}};
  memoryParams.Insert({"repo_path", SC_SERVER_REPO_PATH});

  ScMemoryConfig memoryConfig{config, memoryParams};

  ScMemory::Initialize(memoryConfig.GetParams());

  auto server = std::unique_ptr<ScServer>(new ScServerImpl(
      serverParams.Get<std::string>("host"),
      serverParams.Get<sc_uint32>("port"),
      serverParams.Get<sc_bool>("sync_actions")));

  server->Run();
  server->Stop();
  ScMemory::Shutdown();
}

TEST_F(ScServerTest, Connection)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();
  client.Stop();
}

TEST_F(ScServerTest, HealthcheckOK)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = R"({"type": "healthcheck"})";
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  EXPECT_TRUE(response.get<std::string>() == "OK");

  client.Stop();
}

TEST_F(ScServerTestWithoutParallelMode, HealthcheckOK)
{
  ScClient client;
  EXPECT_TRUE(client.Connect(m_server->GetUri()));
  client.Run();

  std::string const payloadString = R"({"type": "healthcheck"})";
  EXPECT_TRUE(client.Send(payloadString));

  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  EXPECT_EQ(response.get<std::string>(), "OK");

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
            << elapsed.count() - WAIT_BETWEEN_CONNECT_DISCONNECT
                   - 2. * (double)CONNECTIONS * WAIT_BETWEEN_CLIENTS_CONNECT
            << " ms\n";
}

TEST_F(ScServerTest, OneHundredConnections)  // 206.55 ms
{
  TEST_N_CONNECTIONS(m_server, 100);
}

TEST_F(ScServerTest, OneThousandConnections)  // 2583.39 ms
{
  TEST_N_CONNECTIONS(m_server, 1000);
}
