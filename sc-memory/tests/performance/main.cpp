/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "benchmark/benchmark.h"

#include "units/memory_create_edge.hpp"
#include "units/memory_create_node.hpp"
#include "units/memory_create_link.hpp"
#include "units/memory_remove_elements.hpp"

#include "units/sc_code_base_vs_extend.hpp"

#include "units/template_search_complex.hpp"
#include "units/template_search_smoke.hpp"

#include <atomic>

template <class BMType>
void BM_MemoryThreaded(benchmark::State & state)
{
  static std::atomic_int ctxNum = { 0 };
  BMType test;
  if (state.thread_index() == 0)
    test.Initialize();

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

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
->Threads(2)
->Iterations(kNodeIters / 2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
->Threads(4)
->Iterations(kNodeIters / 4)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
->Threads(8)
->Iterations(kNodeIters / 8)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
->Threads(16)
->Iterations(kNodeIters / 16)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
->Threads(32)
->Iterations(kNodeIters / 32)
->Unit(benchmark::TimeUnit::kMicrosecond);

int constexpr kLinkIters = 7000;

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
->Threads(2)
->Iterations(kLinkIters / 2)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
->Threads(4)
->Iterations(kLinkIters / 4)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
->Threads(8)
->Iterations(kLinkIters / 8)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
->Threads(16)
->Iterations(kLinkIters / 64)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
->Threads(32)
->Iterations(kLinkIters / 128)
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

BENCHMARK_TEMPLATE(BM_Memory, TestCreateNode)
->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_Memory, TestCreateLink)
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

BENCHMARK_TEMPLATE(BM_MemoryRanged, TestCreateEdge)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(1000)
->Iterations(5000000);

BENCHMARK_TEMPLATE(BM_MemoryRanged, TestRemoveElements)
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
->Arg(5)->Arg(50)->Arg(500);

// SC-code base vs extended
BENCHMARK_TEMPLATE(BM_Template, TestScCodeBase)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_TEMPLATE(BM_Template, TestScCodeExtended)
->Unit(benchmark::TimeUnit::kMicrosecond)
->Arg(1000)->Arg(10000)->Arg(100000);


BENCHMARK_MAIN();
