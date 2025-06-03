/*
* This source file is part of an OSTIS project. For the latest info, see http:ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http:opensource.org/licenses/MIT)
*/

#include "benchmark/benchmark.h"

#include "units/memory_generate_connector.hpp"
#include "units/memory_generate_node.hpp"
#include "units/memory_generate_link.hpp"
#include "units/memory_iterator_search.hpp"
#include "units/memory_search_link_by_content.hpp"
#include "units/memory_erase_diff_elements.hpp"
#include "units/memory_erase_set_elements.hpp"

#include "units/memory_erase_elements.hpp"

#include "units/sc_code_base_vs_extend.hpp"

#include "units/template_search_complex.hpp"
#include "units/template_search_smoke.hpp"
#include "units/complex_event.hpp"

#include <atomic>
#include <chrono>

template <class BMType>
void BM_MemoryThreaded(benchmark::State & state)
{
  static std::atomic_int ctxNum = {0};
  BMType test;
  if (state.thread_index() == 0)
    test.Initialize();

  auto start = std::chrono::high_resolution_clock::now();
  uint32_t iterations = 0;
  for (auto t : state)
  {
    state.PauseTiming();
    if (!test.HasContext())
    {
      test.InitContext();
      ctxNum.fetch_add(1);
    }
    state.ResumeTiming();

    test.Run();
    ++iterations;
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end - start;
  std::stringstream stream;
  stream << state.max_iterations << " " << elapsed.count() << std::endl;
  std::cout << stream.str();

  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  if (state.thread_index() == 0)
  {
    while (ctxNum.load() != 0);
    test.Shutdown();
  }
  else
  {
    test.DestroyContext();
    ctxNum.fetch_add(-1);
  }
}

int constexpr kNodeIters = 1000000;

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateNode)
->Threads(1)
->Iterations(kNodeIters)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateNode)
->Threads(2)
->Iterations(kNodeIters / 2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateNode)
->Threads(3)
->Iterations(kNodeIters / 3)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateNode)
->Threads(4)
->Iterations(kNodeIters / 4)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateNode)
->Threads(8)
->Iterations(kNodeIters / 8)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateNode)
->Threads(16)
->Iterations(kNodeIters / 16)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateNode)
->Threads(32)
->Iterations(kNodeIters / 32)
->Unit(benchmark::TimeUnit::kMicrosecond);

template <class BMType>
void BM_MemoryThreaded2(benchmark::State & state)
{
  static std::atomic_int ctxNum = {0};
  BMType test;
  if (state.thread_index() == 0)
    test.Initialize(state.range(0));

  auto start = std::chrono::high_resolution_clock::now();
  uint32_t iterations = 0;
  for (auto t : state)
  {
    state.PauseTiming();
    if (!test.HasContext())
    {
      test.InitContext();
      ctxNum.fetch_add(1);
    }
    state.ResumeTiming();

    test.Run();
    ++iterations;
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end - start;
  std::stringstream stream;
  stream << state.max_iterations << " " << elapsed.count() << std::endl;
  std::cout << stream.str();

  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  if (state.thread_index() == 0)
  {
    while (ctxNum.load() != 0);
    test.Shutdown();
  }
  else
  {
    test.DestroyContext();
    ctxNum.fetch_add(-1);
  }
}

int constexpr kEdgeIters = 1000000;

int constexpr kEdgeNodesIters1 = 100;

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(1)
->Iterations(kEdgeIters)
->Arg(kEdgeNodesIters1)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(2)
->Iterations(kEdgeIters / 2)
->Arg(kEdgeNodesIters1)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(3)
->Iterations(kEdgeIters / 3)
->Arg(kEdgeNodesIters1)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(4)
->Iterations(kNodeIters / 4)
->Arg(kEdgeNodesIters1)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(8)
->Iterations(kEdgeIters / 8)
->Arg(kEdgeNodesIters1)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(16)
->Iterations(kEdgeIters / 16)
->Arg(kEdgeNodesIters1)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(32)
->Iterations(kEdgeIters / 32)
->Arg(kEdgeNodesIters1)
->Unit(benchmark::TimeUnit::kMicrosecond);

int constexpr kEdgeNodesIters2 = 10000;

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(1)
->Iterations(kEdgeIters)
->Arg(kEdgeNodesIters2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(2)
->Iterations(kEdgeIters / 2)
->Arg(kEdgeNodesIters2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(3)
->Iterations(kEdgeIters / 3)
->Arg(kEdgeNodesIters2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(4)
->Iterations(kNodeIters / 4)
->Arg(kEdgeNodesIters2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(8)
->Iterations(kEdgeIters / 8)
->Arg(kEdgeNodesIters2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(16)
->Iterations(kEdgeIters / 16)
->Arg(kEdgeNodesIters2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(32)
->Iterations(kEdgeIters / 32)
->Arg(kEdgeNodesIters2)
->Unit(benchmark::TimeUnit::kMicrosecond);

int constexpr kEdgeNodesIters3 = 1000000;

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(1)
->Iterations(kEdgeIters)
->Arg(kEdgeNodesIters3)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(2)
->Iterations(kEdgeIters / 2)
->Arg(kEdgeNodesIters3)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(3)
->Iterations(kEdgeIters / 3)
->Arg(kEdgeNodesIters3)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(4)
->Iterations(kNodeIters / 4)
->Arg(kEdgeNodesIters3)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(8)
->Iterations(kEdgeIters / 8)
->Arg(kEdgeNodesIters3)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(16)
->Iterations(kEdgeIters / 16)
->Arg(kEdgeNodesIters3)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestGenerateConnector)
->Threads(32)
->Iterations(kEdgeIters / 32)
->Arg(kEdgeNodesIters3)
->Unit(benchmark::TimeUnit::kMicrosecond);

int constexpr kLinkIters = 1000000;

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateLink)
->Threads(1)
->Iterations(kLinkIters)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateLink)
->Threads(2)
->Iterations(kLinkIters / 2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateLink)
->Threads(3)
->Iterations(kLinkIters / 3)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateLink)
->Threads(4)
->Iterations(kLinkIters / 4)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateLink)
->Threads(8)
->Iterations(kLinkIters / 8)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateLink)
->Threads(16)
->Iterations(kLinkIters / 64)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestGenerateLink)
->Threads(32)
->Iterations(kLinkIters / 128)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateSameLink)
->Threads(2)
->Iterations(kLinkIters / 2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateSameLink)
->Threads(4)
->Iterations(kLinkIters / 4)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateSameLink)
->Threads(8)
->Iterations(kLinkIters / 8)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateSameLink)
->Threads(16)
->Iterations(kLinkIters / 64)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateSameLink)
->Threads(32)
->Iterations(kLinkIters / 128)
->Unit(benchmark::TimeUnit::kMicrosecond);

int constexpr kSetPower = 1000000;

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestIteratorSearch)
->Threads(1)
->Iterations(kSetPower * 8 / 1)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestIteratorSearch)
->Threads(2)
->Iterations(kSetPower * 8 / 2)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestIteratorSearch)
->Threads(3)
->Iterations(kSetPower * 8 / 3)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestIteratorSearch)
->Threads(4)
->Iterations(kSetPower * 8 / 4)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestIteratorSearch)
->Threads(8)
->Iterations(kSetPower * 8 / 8)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestSearchLinkByContent)
->Threads(1)
->Iterations(kSetPower)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestSearchLinkByContent)
->Threads(2)
->Iterations(kSetPower / 2)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestSearchLinkByContent)
->Threads(3)
->Iterations(kSetPower / 3)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestSearchLinkByContent)
->Threads(4)
->Iterations(kSetPower / 4)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestSearchLinkByContent)
->Threads(8)
->Iterations(kSetPower / 8)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestEraseDiffElements)
->Threads(1)
->Iterations(kSetPower)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestEraseDiffElements)
->Threads(2)
->Iterations(kSetPower / 2)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestEraseDiffElements)
->Threads(3)
->Iterations(kSetPower / 3)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestEraseDiffElements)
->Threads(4)
->Iterations(kSetPower / 4)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestEraseDiffElements)
->Threads(8)
->Iterations(kSetPower / 8)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestEraseSetElements)
->Threads(1)
->Iterations(kSetPower)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestEraseSetElements)
->Threads(2)
->Iterations(kSetPower / 2)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestEraseSetElements)
->Threads(3)
->Iterations(kSetPower / 3)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestEraseSetElements)
->Threads(4)
->Iterations(kSetPower / 4)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded2, TestEraseSetElements)
->Threads(8)
->Iterations(kSetPower / 8)
->Arg(kSetPower)
->Unit(benchmark::TimeUnit::kMicrosecond);

// ------------------------------------
template <class BMType>
void BM_Memory(benchmark::State & state)
{
  BMType test;
  test.Initialize();
  uint32_t iterations = 0;
  for (auto t : state)
  {
    test.Run();
    ++iterations;
  }
  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  test.Shutdown();
}

BENCHMARK_TEMPLATE(BM_Memory, TestGenerateNode)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_Memory, TestGenerateLink)
->Unit(benchmark::TimeUnit::kMicrosecond);

template <class BMType>
void BM_MemoryRanged(benchmark::State & state)
{
  BMType test;
  test.Initialize(state.range(0));
  uint32_t iterations = 0;
  for (auto t : state)
  {
    test.Run();
    ++iterations;
  }
  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  test.Shutdown();
}

BENCHMARK_TEMPLATE(BM_MemoryRanged, TestGenerateConnector)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(1000)
->Iterations(5000000);

BENCHMARK_TEMPLATE(BM_MemoryRanged, TestEraseElements)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(10)->Arg(100)->Arg(1000)
->Iterations(5000);

// ------------------------------------
template <class BMType>
void BM_Template(benchmark::State & state)
{
  BMType test;
  test.Initialize(state.range(0));
  uint32_t iterations = 0;
  for (auto t : state)
  {
    if (!test.Run())
      state.SkipWithError("Empty result");

    ++iterations;
  }
  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  test.Shutdown();
}

BENCHMARK_TEMPLATE(BM_Template, TestTemplateSearchSmoke)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(5)->Arg(50)->Arg(500);

BENCHMARK_TEMPLATE(BM_Template, TestTemplateSearchComplex)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(5)->Arg(50);

// SC-code base vs extended
BENCHMARK_TEMPLATE(BM_Template, TestScCodeBase)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(5)->Arg(50)->Arg(500);

BENCHMARK_TEMPLATE(BM_Template, TestScCodeExtended)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(5)->Arg(50)->Arg(500);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, ComplexEventTest)
->Threads(1)
->Iterations(kNodeIters)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, ComplexEventTest)
->Threads(2)
->Iterations(kNodeIters / 2)
->Unit(benchmark::TimeUnit::kMicrosecond);


BENCHMARK_MAIN();
