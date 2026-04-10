# FetchContent lets this repository download and configure external
# dependencies as part of the CMake configure step.
include(FetchContent)

# Catch2 is our unit-testing framework.
# We pin a tag so the build is reproducible instead of floating on HEAD.
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.4
)

# Google Benchmark is our microbenchmark framework.
# It gives us a standard harness for timing small operations repeatedly.
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
)

# We only need the benchmark library itself, not benchmark's own test suite.
# FORCE is acceptable here because this repository wants a deterministic local
# dependency configuration.
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

# Actually download/configure both dependencies so their targets become usable
# later in the top-level CMakeLists.txt.
FetchContent_MakeAvailable(Catch2 benchmark)
