#include <benchmark/benchmark.h>

static void BM_week01_placeholder(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(state.iterations());
    }
}

BENCHMARK(BM_week01_placeholder);
