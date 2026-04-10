#include <benchmark/benchmark.h>

#include "core/containers/static_vector.hpp"

static void BM_static_vector_push(benchmark::State& state) {
    for (auto _ : state) {
        cppml::static_vector<int, 1024> values;
        for (int i = 0; i < 512; ++i) {
            values.push_back(i);
        }
        benchmark::DoNotOptimize(values);
    }
}

BENCHMARK(BM_static_vector_push);
