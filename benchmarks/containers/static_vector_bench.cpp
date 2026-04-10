#include <benchmark/benchmark.h>

#include "core/containers/static_vector.hpp"

// A benchmark function is just a loop body that Google Benchmark will execute
// many times in a controlled way.
static void BM_static_vector_push(benchmark::State& state) {
    // The framework controls how many outer iterations are needed for a stable timing.
    for (auto _ : state) {
        // Recreate the container every iteration so we benchmark construction plus pushes
        // under the same fixed-capacity conditions each time.
        cppml::static_vector<int, 1024> values;
        // Push a stable number of integers so each iteration has comparable work.
        for (int i = 0; i < 512; ++i) {
            values.push_back(i);
        }
        // Prevent the optimizer from deleting the whole loop as "unused work".
        benchmark::DoNotOptimize(values);
    }
}

// Register the benchmark so benchmark_main can discover and run it.
BENCHMARK(BM_static_vector_push);
