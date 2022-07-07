/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "benchmark/benchmark.h"

#include "units/sc_server_create_edge.hpp"
#include "units/sc_server_create_node.hpp"
#include "units/sc_server_create_link.hpp"
#include "units/sc_server_remove_elements.hpp"

#include <atomic>

template <class BMType>
void BM_ServerThreaded(benchmark::State & state)
{
  static std::atomic_int clientsNum = { 0 };
  BMType test;
  if (state.thread_index() == 0)
    test.Initialize();

  while (test.m_server == nullptr);

  std::unique_ptr<ScClient> client = std::make_unique<ScClient>();
  client->Connect(test.m_server->GetUri());
  client->Run();

  clientsNum.fetch_add(1);

  uint32_t iterations = 0;
  for (auto t : state)
  {
    SC_UNUSED(t);
    test.Run(client);
    ++iterations;
  }

  client->Stop();
  client = nullptr;
  SC_UNUSED(client);

  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  if (state.thread_index() == 0)
  {
    clientsNum.fetch_add(-1);

    while (clientsNum.load() != 0);
    test.Shutdown();
  }
  else
  {
    clientsNum.fetch_add(-1);
  }
}

int constexpr kNodeIters = 100;

BENCHMARK_TEMPLATE(BM_ServerThreaded, TestCreateNode)
->Threads(2)
->Iterations(kNodeIters / 2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_ServerThreaded, TestCreateNode)
->Threads(4)
->Iterations(kNodeIters / 4)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_ServerThreaded, TestCreateNode)
->Threads(8)
->Iterations(kNodeIters / 8)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_ServerThreaded, TestCreateNode)
->Threads(16)
->Iterations(kNodeIters / 16)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_ServerThreaded, TestCreateNode)
->Threads(32)
->Iterations(kNodeIters / 32)
->Unit(benchmark::TimeUnit::kMicrosecond);

int constexpr kLinkIters = 7000;

BENCHMARK_TEMPLATE(BM_ServerThreaded, TestCreateLink)
->Threads(2)
->Iterations(kLinkIters / 2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_ServerThreaded, TestCreateLink)
->Threads(4)
->Iterations(kLinkIters / 4)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_ServerThreaded, TestCreateLink)
->Threads(8)
->Iterations(kLinkIters / 8)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_ServerThreaded, TestCreateLink)
->Threads(16)
->Iterations(kLinkIters / 64)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_ServerThreaded, TestCreateLink)
->Threads(32)
->Iterations(kLinkIters / 128)
->Unit(benchmark::TimeUnit::kMicrosecond);


// ------------------------------------
template <class BMType>
void BM_Server(benchmark::State & state)
{
  BMType test;
  test.Initialize();
  uint32_t iterations = 0;

  std::unique_ptr<ScClient> client = std::make_unique<ScClient>();
  client->Connect(test.m_server->GetUri());
  client->Run();
  test.WaitServer();

  for (auto t : state)
  {
    SC_UNUSED(t);
    test.Run(client);
    ++iterations;
  }

  test.WaitServer();
  client->Stop();
  client = nullptr;
  SC_UNUSED(client);

  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  test.Shutdown();
}

BENCHMARK_TEMPLATE(BM_Server, TestCreateNode)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_Server, TestCreateLink)
->Unit(benchmark::TimeUnit::kMicrosecond);


// ------------------------------------
template <class BMType>
void BM_ServerRanged(benchmark::State & state)
{
  BMType test;
  test.Initialize(state.range(0));
  uint32_t iterations = 0;

  std::unique_ptr<ScClient> client = std::make_unique<ScClient>();
  client->Connect(test.m_server->GetUri());
  client->Run();
  test.WaitServer();

  for (auto t : state)
  {
    SC_UNUSED(t);
    test.Run(client);
    ++iterations;
  }

  test.WaitServer();
  client->Stop();
  client = nullptr;
  SC_UNUSED(client);

  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  test.Shutdown();
}

BENCHMARK_TEMPLATE(BM_ServerRanged, TestCreateEdge)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(1000)
->Iterations(5000000);

BENCHMARK_TEMPLATE(BM_ServerRanged, TestRemoveElements)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(10)->Arg(100)->Arg(1000)
->Iterations(5000);


BENCHMARK_MAIN();
