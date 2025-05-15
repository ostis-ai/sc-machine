#include "benchmark/benchmark.h"
#include "units/transaction/test_sc_transaction_add_node.hpp"

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