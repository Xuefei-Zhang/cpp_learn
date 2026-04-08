# C++ Engineering Mastery Lab Implementation Plan

> **For agentic workers:** REQUIRED: Use superpowers:subagent-driven-development (if subagents available) or superpowers:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the full 12-week C++ Engineering Mastery Lab repository, including repo scaffolding, weekly mission docs, glossary checkpoints, reusable core/runtime modules, tests, benchmarks, and the final mini task runtime + resource-aware execution toolkit.

**Architecture:** This plan builds one cumulative C++20 codebase in four chunks. Early tasks establish CMake, tests, sanitizers, notes, and ownership-oriented utility types; middle tasks add generic interfaces, callable abstractions, iterators, allocators, and concurrency primitives; final tasks integrate those pieces into a small execution system with measurable behavior and explainable tradeoffs.

**Tech Stack:** C++20, CMake, CTest, Catch2, Google Benchmark, standard library threading primitives, AddressSanitizer, UndefinedBehaviorSanitizer, ThreadSanitizer, clang-tidy (optional), markdown notes/specs.

---

## Repository File Map

### Build and toolchain files
- Create: `CMakeLists.txt`
- Create: `cmake/ProjectOptions.cmake`
- Create: `cmake/Sanitizers.cmake`
- Create: `cmake/Warnings.cmake`
- Create: `cmake/Dependencies.cmake`

### Documentation and repo entry files
- Create: `README.md`
- Create: `docs/glossary/week-01-core-terms.md`
- Create: `docs/glossary/week-04-generic-design-terms.md`
- Create: `docs/glossary/week-07-runtime-and-allocation-terms.md`
- Create: `docs/glossary/week-10-system-integration-terms.md`
- Create: `notes/weekly/week-01.md` through `notes/weekly/week-12.md`
- Create: `missions/week-01-project-bootstrap-and-value-semantics/README.md`
- Create: `missions/week-02-raii-and-resource-management/README.md`
- Create: `missions/week-03-small-container-and-exception-safety/README.md`
- Create: `missions/week-04-templates-constraints-and-generic-apis/README.md`
- Create: `missions/week-05-callable-design-and-type-erasure/README.md`
- Create: `missions/week-06-iterators-views-and-range-oriented-design/README.md`
- Create: `missions/week-07-allocators-and-memory-resource-thinking/README.md`
- Create: `missions/week-08-concurrency-fundamentals/README.md`
- Create: `missions/week-09-exception-safety-and-performance-accountability/README.md`
- Create: `missions/week-10-task-runtime-skeleton/README.md`
- Create: `missions/week-11-resource-aware-execution-pipeline/README.md`
- Create: `missions/week-12-productization-explanation-and-evidence/README.md`

### Core module files
- Create: `core/utility/span_like.hpp`
- Create: `core/memory/scope_guard.hpp`
- Create: `core/memory/unique_resource.hpp`
- Create: `core/containers/static_vector.hpp`
- Create: `core/function/function_ref.hpp`
- Create: `core/function/small_function.hpp`
- Create: `core/ranges/iterator_range.hpp`
- Create: `core/ranges/filter_view.hpp`

### Runtime files
- Create: `runtime/allocators/monotonic_arena.hpp`
- Create: `runtime/concurrency/thread_safe_queue.hpp`
- Create: `runtime/concurrency/thread_pool.hpp`

### System integration files
- Create: `systems/task_runtime/task.hpp`
- Create: `systems/task_runtime/executor.hpp`
- Create: `systems/resource_cache/resource_registry.hpp`
- Create: `systems/event_pipeline/pipeline.hpp`

### Test files
- Create: `tests/core/span_like.test.cpp`
- Create: `tests/core/scope_guard.test.cpp`
- Create: `tests/core/unique_resource.test.cpp`
- Create: `tests/core/static_vector.test.cpp`
- Create: `tests/core/function_ref.test.cpp`
- Create: `tests/core/small_function.test.cpp`
- Create: `tests/core/filter_view.test.cpp`
- Create: `tests/runtime/monotonic_arena.test.cpp`
- Create: `tests/runtime/thread_safe_queue.test.cpp`
- Create: `tests/runtime/thread_pool.test.cpp`
- Create: `tests/systems/executor.test.cpp`
- Create: `tests/systems/resource_registry.test.cpp`
- Create: `tests/systems/pipeline.test.cpp`

### Benchmark files
- Create: `benchmarks/containers/static_vector_bench.cpp`
- Create: `benchmarks/allocators/monotonic_arena_bench.cpp`
- Create: `benchmarks/concurrency/thread_pool_bench.cpp`

---

## Commit Strategy

Use this commit rhythm throughout the plan:

1. `test:` add failing tests and scaffolding for one week/task
2. `feat:` add the minimal implementation that makes the tests pass
3. `refactor:` harden APIs, add notes/benchmarks/mission docs without changing behavior

If a week is tiny, combine steps 2 and 3. Do not combine two different weeks into one commit.

---

## Chunk 1: Bootstrap, Ownership, and Lifetime Foundations

### Task 1: Bootstrap the repository and Week 1 learning slice

**Files:**
- Create: `CMakeLists.txt`
- Create: `cmake/ProjectOptions.cmake`
- Create: `cmake/Sanitizers.cmake`
- Create: `cmake/Warnings.cmake`
- Create: `cmake/Dependencies.cmake`
- Create: `README.md`
- Create: `core/utility/span_like.hpp`
- Create: `tests/core/span_like.test.cpp`
- Create: `benchmarks/containers/static_vector_bench.cpp`
- Create: `docs/glossary/week-01-core-terms.md`
- Create: `notes/weekly/week-01.md`
- Create: `missions/week-01-project-bootstrap-and-value-semantics/README.md`

- [ ] **Step 1: Write the failing Week 1 test**

```cpp
#include <catch2/catch_test_macros.hpp>
#include "core/utility/span_like.hpp"

TEST_CASE("span_like exposes size and element access", "[week01][span]") {
    int values[] = {1, 2, 3};
    cppml::span_like<int> span(values, 3);

    REQUIRE(span.size() == 3);
    REQUIRE(span[0] == 1);
    REQUIRE(span[2] == 3);
}
```

- [ ] **Step 2: Run the test target to verify it fails**

Run:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure -R week01
```
Expected: configure/build fails because Catch2 dependency and `core/utility/span_like.hpp` do not exist yet.

- [ ] **Step 3: Add the minimal build system and dependencies**

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(cpp_engineering_mastery_lab LANGUAGES CXX)

include(cmake/ProjectOptions.cmake)
include(cmake/Warnings.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/Dependencies.cmake)

enable_testing()
add_subdirectory(tests)
add_subdirectory(benchmarks)
```

```cmake
# cmake/Dependencies.cmake
include(FetchContent)
FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.5.4)
FetchContent_Declare(benchmark GIT_REPOSITORY https://github.com/google/benchmark.git GIT_TAG v1.8.3)
FetchContent_MakeAvailable(Catch2 benchmark)
```

- [ ] **Step 4: Add the minimal Week 1 implementation**

```cpp
#pragma once

#include <cstddef>
#include <stdexcept>

namespace cppml {
template <class T>
class span_like {
public:
    using value_type = T;

    constexpr span_like(T* data, std::size_t size) noexcept : data_(data), size_(size) {}

    [[nodiscard]] constexpr std::size_t size() const noexcept { return size_; }
    [[nodiscard]] constexpr T& operator[](std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("span_like index out of range");
        }
        return data_[index];
    }

private:
    T* data_;
    std::size_t size_;
};
}  // namespace cppml
```

- [ ] **Step 5: Add the Week 1 documentation slice**

Write these files with concrete content:
- `README.md`: project purpose, 12-week structure, build/test commands
- `docs/glossary/week-01-core-terms.md`: define translation unit, linkage, owning vs non-owning, trivial type, move, RAII
- `notes/weekly/week-01.md`: explain why `span_like` is non-owning and what bugs it does not prevent
- `missions/week-01-project-bootstrap-and-value-semantics/README.md`: use the six-section mission template from the spec

- [ ] **Step 6: Run verification for Week 1**

Run:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure -R week01
```
Expected: configure succeeds, build succeeds, `week01` test passes.

- [ ] **Step 7: Commit Week 1**

```bash
git init
git add CMakeLists.txt cmake README.md core/utility/span_like.hpp tests/core/span_like.test.cpp docs/glossary/week-01-core-terms.md notes/weekly/week-01.md missions/week-01-project-bootstrap-and-value-semantics/README.md
git commit -m "feat: bootstrap week 1 mastery lab foundation"
```

### Task 2: Build Week 2 RAII primitives

**Files:**
- Create: `core/memory/scope_guard.hpp`
- Create: `core/memory/unique_resource.hpp`
- Create: `tests/core/scope_guard.test.cpp`
- Create: `tests/core/unique_resource.test.cpp`
- Create: `notes/weekly/week-02.md`
- Create: `missions/week-02-raii-and-resource-management/README.md`

- [ ] **Step 1: Write the failing RAII tests**

```cpp
TEST_CASE("scope_guard runs cleanup on scope exit", "[week02][scope_guard]") {
    bool cleaned = false;
    {
        cppml::scope_guard guard([&] { cleaned = true; });
        REQUIRE_FALSE(cleaned);
    }
    REQUIRE(cleaned);
}

TEST_CASE("unique_resource releases exactly once", "[week02][unique_resource]") {
    int releases = 0;
    {
        cppml::unique_resource<int, void(*)(int&)> resource(7, [&](int&) { ++releases; });
        REQUIRE(resource.get() == 7);
    }
    REQUIRE(releases == 1);
}
```

- [ ] **Step 2: Run tests to verify they fail**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week02
```
Expected: compile failure because `scope_guard.hpp` and `unique_resource.hpp` do not exist yet.

- [ ] **Step 3: Implement the minimal RAII types**

```cpp
// scope_guard.hpp
namespace cppml {
template <class F>
class scope_guard {
public:
    explicit scope_guard(F fn) noexcept(std::is_nothrow_move_constructible_v<F>) : fn_(std::move(fn)) {}
    scope_guard(scope_guard&& other) noexcept : fn_(std::move(other.fn_)), active_(std::exchange(other.active_, false)) {}
    scope_guard(const scope_guard&) = delete;
    scope_guard& operator=(const scope_guard&) = delete;
    ~scope_guard() { if (active_) fn_(); }
    void release() noexcept { active_ = false; }
private:
    F fn_;
    bool active_ = true;
};
}
```

```cpp
// unique_resource.hpp
namespace cppml {
template <class R, class D>
class unique_resource {
public:
    unique_resource(R resource, D deleter) noexcept : resource_(std::move(resource)), deleter_(std::move(deleter)) {}
    unique_resource(unique_resource&& other) noexcept
        : resource_(std::move(other.resource_)), deleter_(std::move(other.deleter_)), active_(std::exchange(other.active_, false)) {}
    unique_resource(const unique_resource&) = delete;
    unique_resource& operator=(const unique_resource&) = delete;
    ~unique_resource() { reset(); }
    R& get() noexcept { return resource_; }
    void reset() noexcept { if (active_) { deleter_(resource_); active_ = false; } }
private:
    R resource_;
    D deleter_;
    bool active_ = true;
};
}
```

- [ ] **Step 4: Add failure-path tests and notes**

Extend the tests to cover:
- moved-from guard does not clean up twice
- `unique_resource::reset()` is idempotent
- destructor cleanup still happens when an exception is thrown inside the guarded scope

Create:
- `notes/weekly/week-02.md`
- `missions/week-02-raii-and-resource-management/README.md`

- [ ] **Step 5: Re-run verification**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week02
```
Expected: all `week02` tests pass.

- [ ] **Step 6: Commit Week 2**

```bash
git add core/memory/scope_guard.hpp core/memory/unique_resource.hpp tests/core/scope_guard.test.cpp tests/core/unique_resource.test.cpp notes/weekly/week-02.md missions/week-02-raii-and-resource-management/README.md
git commit -m "feat: add week 2 RAII primitives"
```

### Task 3: Build Week 3 static_vector and first benchmark

**Files:**
- Create: `core/containers/static_vector.hpp`
- Create: `tests/core/static_vector.test.cpp`
- Modify: `benchmarks/containers/static_vector_bench.cpp`
- Create: `notes/weekly/week-03.md`
- Create: `missions/week-03-small-container-and-exception-safety/README.md`

- [ ] **Step 1: Write the failing container tests**

```cpp
TEST_CASE("static_vector pushes and indexes values", "[week03][static_vector]") {
    cppml::static_vector<int, 4> values;
    values.push_back(1);
    values.push_back(2);

    REQUIRE(values.size() == 2);
    REQUIRE(values[1] == 2);
}
```

- [ ] **Step 2: Run the Week 3 tests to verify they fail**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week03
```
Expected: compile failure because `static_vector.hpp` does not exist.

- [ ] **Step 3: Implement the minimal container**

```cpp
namespace cppml {
template <class T, std::size_t Capacity>
class static_vector {
public:
    ~static_vector() { clear(); }

    void push_back(const T& value) {
        if (size_ >= Capacity) throw std::length_error("static_vector full");
        new (&storage_[size_]) T(value);
        ++size_;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] T& operator[](std::size_t index) noexcept { return *std::launder(reinterpret_cast<T*>(&storage_[index])); }
    void clear() noexcept {
        while (size_ > 0) {
            --size_;
            std::destroy_at(std::launder(reinterpret_cast<T*>(&storage_[size_])));
        }
    }

private:
    alignas(T) std::byte storage_[sizeof(T) * Capacity]{};
    std::size_t size_ = 0;
};
}
```

- [ ] **Step 4: Add the benchmark and edge tests**

Add tests for:
- overflow throws
- clear destroys elements
- move-only element support if feasible in Week 3

Add benchmark skeleton:

```cpp
static void BM_static_vector_push(benchmark::State& state) {
    for (auto _ : state) {
        cppml::static_vector<int, 1024> values;
        for (int i = 0; i < 512; ++i) values.push_back(i);
        benchmark::DoNotOptimize(values);
    }
}
BENCHMARK(BM_static_vector_push);
```

- [ ] **Step 5: Verify Week 3**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week03
./build/benchmarks/static_vector_bench --benchmark_min_time=0.01
```
Expected: test passes, benchmark runs and prints at least one timing row.

- [ ] **Step 6: Commit Week 3**

```bash
git add core/containers/static_vector.hpp tests/core/static_vector.test.cpp benchmarks/containers/static_vector_bench.cpp notes/weekly/week-03.md missions/week-03-small-container-and-exception-safety/README.md
git commit -m "feat: add week 3 static_vector and benchmark"
```

---

## Chunk 2: Generic Interfaces, Callable Design, and Ranges

### Task 4: Add Week 4 concepts and constrained APIs

**Files:**
- Modify: `core/containers/static_vector.hpp`
- Create: `tests/core/static_vector_concepts.test.cpp`
- Create: `docs/glossary/week-04-generic-design-terms.md`
- Create: `notes/weekly/week-04.md`
- Create: `missions/week-04-templates-constraints-and-generic-apis/README.md`

- [ ] **Step 1: Write the failing concept-driven tests**

```cpp
TEST_CASE("static_vector supports constrained emplace_back", "[week04][concepts]") {
    cppml::static_vector<std::string, 4> values;
    values.emplace_back("hello");
    REQUIRE(values[0] == "hello");
}
```

- [ ] **Step 2: Verify the failure**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week04
```
Expected: build fails because `emplace_back` and concept-constrained overloads are missing.

- [ ] **Step 3: Add constrained APIs**

```cpp
template <class... Args>
requires std::constructible_from<T, Args...>
T& emplace_back(Args&&... args) {
    if (size_ >= Capacity) throw std::length_error("static_vector full");
    auto* slot = std::launder(reinterpret_cast<T*>(&storage_[size_]));
    std::construct_at(slot, std::forward<Args>(args)...);
    ++size_;
    return *slot;
}
```

- [ ] **Step 4: Write glossary, note, and mission docs**

Create:
- `docs/glossary/week-04-generic-design-terms.md`
- `notes/weekly/week-04.md`
- `missions/week-04-templates-constraints-and-generic-apis/README.md`

Include definitions for concepts, forwarding reference, overload set, instantiation, and type erasure.

- [ ] **Step 5: Verify Week 4**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week04
```
Expected: Week 4 tests pass and existing Week 3 container tests still pass.

- [ ] **Step 6: Commit Week 4**

```bash
git add core/containers/static_vector.hpp tests/core/static_vector_concepts.test.cpp docs/glossary/week-04-generic-design-terms.md notes/weekly/week-04.md missions/week-04-templates-constraints-and-generic-apis/README.md
git commit -m "feat: add week 4 constrained container APIs"
```

### Task 5: Build Week 5 function_ref and small_function

**Files:**
- Create: `core/function/function_ref.hpp`
- Create: `core/function/small_function.hpp`
- Create: `tests/core/function_ref.test.cpp`
- Create: `tests/core/small_function.test.cpp`
- Create: `notes/weekly/week-05.md`
- Create: `missions/week-05-callable-design-and-type-erasure/README.md`

- [ ] **Step 1: Write the failing callable tests**

```cpp
TEST_CASE("function_ref invokes non-owning callable", "[week05][function_ref]") {
    int total = 0;
    auto lambda = [&](int value) { total += value; };
    cppml::function_ref<void(int)> ref(lambda);
    ref(3);
    REQUIRE(total == 3);
}
```

```cpp
TEST_CASE("small_function stores move-only callable", "[week05][small_function]") {
    auto ptr = std::make_unique<int>(4);
    cppml::small_function<int()> fn([p = std::move(ptr)] { return *p; });
    REQUIRE(fn() == 4);
}
```

- [ ] **Step 2: Run tests to verify failure**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week05
```
Expected: compile failure because callable wrappers do not exist.

- [ ] **Step 3: Implement `function_ref` minimally**

```cpp
template <class R, class... Args>
class function_ref<R(Args...)> {
public:
    template <class F>
    function_ref(F& fn) noexcept
        : object_(std::addressof(fn)), callback_([](void* obj, Args... args) -> R {
              return (*static_cast<F*>(obj))(std::forward<Args>(args)...);
          }) {}

    R operator()(Args... args) const {
        return callback_(object_, std::forward<Args>(args)...);
    }

private:
    void* object_ = nullptr;
    R (*callback_)(void*, Args...) = nullptr;
};
```

- [ ] **Step 4: Implement `small_function` minimally**

Use a fixed-size inline buffer, move-only semantics, and one stored invoker. Keep the first implementation intentionally small: no allocator support, no copy semantics, and a tight capacity such as 32 bytes.

- [ ] **Step 5: Verify Week 5 and write notes**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week05
```
Expected: callable tests pass.

Then create:
- `notes/weekly/week-05.md`
- `missions/week-05-callable-design-and-type-erasure/README.md`

- [ ] **Step 6: Commit Week 5**

```bash
git add core/function/function_ref.hpp core/function/small_function.hpp tests/core/function_ref.test.cpp tests/core/small_function.test.cpp notes/weekly/week-05.md missions/week-05-callable-design-and-type-erasure/README.md
git commit -m "feat: add week 5 callable abstractions"
```

### Task 6: Build Week 6 iterator_range and filter_view

**Files:**
- Create: `core/ranges/iterator_range.hpp`
- Create: `core/ranges/filter_view.hpp`
- Create: `tests/core/filter_view.test.cpp`
- Create: `notes/weekly/week-06.md`
- Create: `missions/week-06-iterators-views-and-range-oriented-design/README.md`

- [ ] **Step 1: Write the failing filter-view test**

```cpp
TEST_CASE("filter_view exposes only matching items", "[week06][ranges]") {
    std::array<int, 5> values{1, 2, 3, 4, 5};
    auto even = cppml::filter_view(values, [](int value) { return value % 2 == 0; });

    std::vector<int> result;
    for (int value : even) result.push_back(value);

    REQUIRE(result == std::vector<int>{2, 4});
}
```

- [ ] **Step 2: Verify failure**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week06
```
Expected: compile failure because range helpers do not exist.

- [ ] **Step 3: Implement minimal range types**

Create an `iterator_range<It>` that stores `begin`/`end`, then create `filter_view` with an iterator that skips non-matching elements in `operator++`.

- [ ] **Step 4: Add docs and notes**

Create:
- `notes/weekly/week-06.md`
- `missions/week-06-iterators-views-and-range-oriented-design/README.md`

Include notes on iterator category choice, lazy evaluation, and abstraction cost.

- [ ] **Step 5: Verify Week 6**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week06
```
Expected: filter-view tests pass.

- [ ] **Step 6: Commit Week 6**

```bash
git add core/ranges/iterator_range.hpp core/ranges/filter_view.hpp tests/core/filter_view.test.cpp notes/weekly/week-06.md missions/week-06-iterators-views-and-range-oriented-design/README.md
git commit -m "feat: add week 6 iterator and view abstractions"
```

---

## Chunk 3: Allocators, Concurrency, and Runtime Hardening

### Task 7: Build Week 7 monotonic_arena

**Files:**
- Create: `runtime/allocators/monotonic_arena.hpp`
- Create: `tests/runtime/monotonic_arena.test.cpp`
- Create: `benchmarks/allocators/monotonic_arena_bench.cpp`
- Create: `docs/glossary/week-07-runtime-and-allocation-terms.md`
- Create: `notes/weekly/week-07.md`
- Create: `missions/week-07-allocators-and-memory-resource-thinking/README.md`

- [ ] **Step 1: Write the failing arena test**

```cpp
TEST_CASE("monotonic_arena hands out sequential allocations and reset reuses storage", "[week07][arena]") {
    cppml::monotonic_arena arena(1024);
    void* a = arena.allocate(64, alignof(std::max_align_t));
    void* b = arena.allocate(64, alignof(std::max_align_t));
    REQUIRE(a != nullptr);
    REQUIRE(b != nullptr);
    REQUIRE(a != b);
    arena.reset();
    void* c = arena.allocate(64, alignof(std::max_align_t));
    REQUIRE(c == a);
}
```

- [ ] **Step 2: Verify failure**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week07
```
Expected: compile failure because `monotonic_arena.hpp` does not exist.

- [ ] **Step 3: Implement the arena minimally**

Use a `std::vector<std::byte>` backing buffer, maintain a current offset, align each request with `std::align`, and expose `reset()`.

- [ ] **Step 4: Add benchmark and glossary**

Create:
- `benchmarks/allocators/monotonic_arena_bench.cpp`
- `docs/glossary/week-07-runtime-and-allocation-terms.md`
- `notes/weekly/week-07.md`
- `missions/week-07-allocators-and-memory-resource-thinking/README.md`

Benchmark compare repeated small allocations from the arena vs `::operator new`.

- [ ] **Step 5: Verify Week 7**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week07
./build/benchmarks/monotonic_arena_bench --benchmark_min_time=0.01
```
Expected: tests pass and benchmark prints at least one comparison.

- [ ] **Step 6: Commit Week 7**

```bash
git add runtime/allocators/monotonic_arena.hpp tests/runtime/monotonic_arena.test.cpp benchmarks/allocators/monotonic_arena_bench.cpp docs/glossary/week-07-runtime-and-allocation-terms.md notes/weekly/week-07.md missions/week-07-allocators-and-memory-resource-thinking/README.md
git commit -m "feat: add week 7 monotonic arena"
```

### Task 8: Build Week 8 thread_safe_queue and thread_pool

**Files:**
- Create: `runtime/concurrency/thread_safe_queue.hpp`
- Create: `runtime/concurrency/thread_pool.hpp`
- Create: `tests/runtime/thread_safe_queue.test.cpp`
- Create: `tests/runtime/thread_pool.test.cpp`
- Create: `benchmarks/concurrency/thread_pool_bench.cpp`
- Create: `notes/weekly/week-08.md`
- Create: `missions/week-08-concurrency-fundamentals/README.md`

- [ ] **Step 1: Write the failing concurrency tests**

```cpp
TEST_CASE("thread_safe_queue transfers values between threads", "[week08][queue]") {
    cppml::thread_safe_queue<int> queue;
    std::thread producer([&] { queue.push(7); });
    auto value = queue.wait_and_pop();
    producer.join();
    REQUIRE(value == 7);
}
```

```cpp
TEST_CASE("thread_pool executes submitted work", "[week08][pool]") {
    cppml::thread_pool pool(2);
    std::atomic<int> total = 0;
    auto future = pool.submit([&] { total.fetch_add(5); return 5; });
    REQUIRE(future.get() == 5);
    REQUIRE(total.load() == 5);
}
```

- [ ] **Step 2: Verify failure**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week08
```
Expected: compile failure because queue and pool do not exist.

- [ ] **Step 3: Implement the queue minimally**

Use `std::mutex`, `std::condition_variable`, `std::queue<T>`, `push`, `wait_and_pop`, and `close`.

- [ ] **Step 4: Implement the pool minimally**

Use worker threads, a task queue of `small_function<void()>`, and `std::packaged_task` for `submit`.

- [ ] **Step 5: Add benchmark, notes, and lifetime tests**

Add tests for:
- shutdown wakes waiting workers
- submitting after shutdown fails predictably
- queued tasks finish before destructor returns

Then run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week08
./build/benchmarks/thread_pool_bench --benchmark_min_time=0.01
```
Expected: concurrency tests pass; benchmark runs.

- [ ] **Step 6: Commit Week 8**

```bash
git add runtime/concurrency/thread_safe_queue.hpp runtime/concurrency/thread_pool.hpp tests/runtime/thread_safe_queue.test.cpp tests/runtime/thread_pool.test.cpp benchmarks/concurrency/thread_pool_bench.cpp notes/weekly/week-08.md missions/week-08-concurrency-fundamentals/README.md
git commit -m "feat: add week 8 queue and thread pool"
```

### Task 9: Harden Week 9 exception guarantees and sanitizer targets

**Files:**
- Modify: `core/containers/static_vector.hpp`
- Modify: `core/function/small_function.hpp`
- Modify: `runtime/concurrency/thread_pool.hpp`
- Create: `tests/core/exception_guarantees.test.cpp`
- Create: `notes/weekly/week-09.md`
- Create: `missions/week-09-exception-safety-and-performance-accountability/README.md`

- [ ] **Step 1: Write the failing exception guarantee tests**

```cpp
TEST_CASE("static_vector preserves original elements when emplacement throws", "[week09][exceptions]") {
    struct ThrowOnSecond {
        static inline int count = 0;
        ThrowOnSecond() {
            if (++count == 2) throw std::runtime_error("boom");
        }
    };

    cppml::static_vector<ThrowOnSecond, 4> values;
    values.emplace_back();
    REQUIRE_THROWS(values.emplace_back());
    REQUIRE(values.size() == 1);
}
```

- [ ] **Step 2: Verify failure**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week09
```
Expected: tests fail because the current implementations do not yet preserve the desired guarantees.

- [ ] **Step 3: Fix root causes minimally**

Harden:
- `static_vector::emplace_back` so size increments only after successful construction
- `small_function` move/destroy logic so moved-from and destruction states are valid
- `thread_pool` shutdown path so tasks and worker lifetimes are explicit and safe

- [ ] **Step 4: Add sanitizer presets and docs**

Extend `cmake/Sanitizers.cmake` with explicit ASan/UBSan and TSan options. Create:
- `notes/weekly/week-09.md`
- `missions/week-09-exception-safety-and-performance-accountability/README.md`

- [ ] **Step 5: Verify Week 9**

Run:
```bash
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DCPPML_ENABLE_ASAN=ON -DCPPML_ENABLE_UBSAN=ON
cmake --build build-asan
ctest --test-dir build-asan --output-on-failure
cmake -S . -B build-tsan -DCMAKE_BUILD_TYPE=Debug -DCPPML_ENABLE_TSAN=ON
cmake --build build-tsan
ctest --test-dir build-tsan --output-on-failure -R week08
```
Expected: sanitizer-enabled builds pass relevant tests.

- [ ] **Step 6: Commit Week 9**

```bash
git add core/containers/static_vector.hpp core/function/small_function.hpp runtime/concurrency/thread_pool.hpp tests/core/exception_guarantees.test.cpp cmake/Sanitizers.cmake notes/weekly/week-09.md missions/week-09-exception-safety-and-performance-accountability/README.md
git commit -m "refactor: harden week 9 exception and sanitizer guarantees"
```

---

## Chunk 4: System Integration and Final Productization

### Task 10: Build Week 10 task and executor skeleton

**Files:**
- Create: `systems/task_runtime/task.hpp`
- Create: `systems/task_runtime/executor.hpp`
- Create: `tests/systems/executor.test.cpp`
- Create: `docs/glossary/week-10-system-integration-terms.md`
- Create: `notes/weekly/week-10.md`
- Create: `missions/week-10-task-runtime-skeleton/README.md`

- [ ] **Step 1: Write the failing executor test**

```cpp
TEST_CASE("executor schedules tasks on the thread pool", "[week10][executor]") {
    cppml::thread_pool pool(2);
    cppml::executor exec(pool);

    auto future = exec.schedule([] { return 42; });
    REQUIRE(future.get() == 42);
}
```

- [ ] **Step 2: Verify failure**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week10
```
Expected: compile failure because `task.hpp` and `executor.hpp` do not exist.

- [ ] **Step 3: Implement task and executor minimally**

Create a thin `task<R>` wrapper around a callable or future-producing function, then create `executor` as a lightweight adapter over `thread_pool::submit`.

- [ ] **Step 4: Write glossary, notes, and mission docs**

Create:
- `docs/glossary/week-10-system-integration-terms.md`
- `notes/weekly/week-10.md`
- `missions/week-10-task-runtime-skeleton/README.md`

- [ ] **Step 5: Verify Week 10**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week10
```
Expected: executor tests pass.

- [ ] **Step 6: Commit Week 10**

```bash
git add systems/task_runtime/task.hpp systems/task_runtime/executor.hpp tests/systems/executor.test.cpp docs/glossary/week-10-system-integration-terms.md notes/weekly/week-10.md missions/week-10-task-runtime-skeleton/README.md
git commit -m "feat: add week 10 task runtime skeleton"
```

### Task 11: Build Week 11 resource registry and pipeline integration

**Files:**
- Create: `systems/resource_cache/resource_registry.hpp`
- Create: `systems/event_pipeline/pipeline.hpp`
- Create: `tests/systems/resource_registry.test.cpp`
- Create: `tests/systems/pipeline.test.cpp`
- Create: `notes/weekly/week-11.md`
- Create: `missions/week-11-resource-aware-execution-pipeline/README.md`

- [ ] **Step 1: Write the failing integration tests**

```cpp
TEST_CASE("resource_registry lazily creates and reuses resources", "[week11][registry]") {
    cppml::resource_registry<std::string, int> registry;
    int creates = 0;

    auto& first = registry.get_or_create("alpha", [&] {
        ++creates;
        return 7;
    });
    auto& second = registry.get_or_create("alpha", [&] {
        ++creates;
        return 9;
    });

    REQUIRE(&first == &second);
    REQUIRE(creates == 1);
}
```

```cpp
TEST_CASE("pipeline runs staged work and propagates failures", "[week11][pipeline]") {
    cppml::pipeline<int> pipe;
    pipe.then([](int value) { return value + 1; })
        .then([](int value) { return value * 2; });

    REQUIRE(pipe.run(3).value() == 8);
}
```

- [ ] **Step 2: Verify failure**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week11
```
Expected: compile failure because registry and pipeline types do not exist.

- [ ] **Step 3: Implement the minimal system pieces**

Create:
- a `resource_registry<Key, Value>` backed by `std::unordered_map`
- a `pipeline<T>` that stores a vector of staged operations and returns `std::expected<T, std::string>` from `run`

- [ ] **Step 4: Add failure-path tests and notes**

Extend tests to cover:
- resource factory throws
- pipeline stage throws and returns an error
- registry ownership stays internal and references remain valid until erase/reset

Create:
- `notes/weekly/week-11.md`
- `missions/week-11-resource-aware-execution-pipeline/README.md`

- [ ] **Step 5: Verify Week 11**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure -R week11
```
Expected: system integration tests pass.

- [ ] **Step 6: Commit Week 11**

```bash
git add systems/resource_cache/resource_registry.hpp systems/event_pipeline/pipeline.hpp tests/systems/resource_registry.test.cpp tests/systems/pipeline.test.cpp notes/weekly/week-11.md missions/week-11-resource-aware-execution-pipeline/README.md
git commit -m "feat: add week 11 resource-aware pipeline"
```

### Task 12: Finish Week 12 productization and final validation

**Files:**
- Modify: `README.md`
- Create: `notes/weekly/week-12.md`
- Create: `notes/interview/final-project-summary.md`
- Create: `notes/tradeoffs/final-tradeoffs.md`
- Create: `missions/week-12-productization-explanation-and-evidence/README.md`
- Modify: `benchmarks/containers/static_vector_bench.cpp`
- Modify: `benchmarks/allocators/monotonic_arena_bench.cpp`
- Modify: `benchmarks/concurrency/thread_pool_bench.cpp`

- [ ] **Step 1: Write the final failing documentation/integration checklist**

Create a checklist in `missions/week-12-productization-explanation-and-evidence/README.md` that requires:
- repository overview
- build/test/benchmark commands
- architecture summary
- final tradeoff summary
- benchmark evidence links or pasted excerpts

- [ ] **Step 2: Run the full suite before final polish**

Run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure
./build/benchmarks/static_vector_bench --benchmark_min_time=0.01
./build/benchmarks/monotonic_arena_bench --benchmark_min_time=0.01
./build/benchmarks/thread_pool_bench --benchmark_min_time=0.01
```
Expected: all implemented tests pass and all three benchmark executables run.

- [ ] **Step 3: Update the repo-facing documentation**

Update `README.md` so it includes:
- the project goal
- repo layout
- week-by-week map
- build/test/sanitizer/benchmark commands
- how the final mini runtime is assembled from prior modules

- [ ] **Step 4: Write final notes**

Create:
- `notes/weekly/week-12.md`
- `notes/interview/final-project-summary.md`
- `notes/tradeoffs/final-tradeoffs.md`

Make each note concrete. The final tradeoff note should discuss ownership, exception safety, abstraction cost, allocation strategy, and shutdown semantics.

- [ ] **Step 5: Run final verification**

Run:
```bash
cmake -S . -B build-final -DCMAKE_BUILD_TYPE=Release
cmake --build build-final
ctest --test-dir build-final --output-on-failure
./build-final/benchmarks/static_vector_bench --benchmark_min_time=0.01
./build-final/benchmarks/monotonic_arena_bench --benchmark_min_time=0.01
./build-final/benchmarks/thread_pool_bench --benchmark_min_time=0.01
```
Expected: release build succeeds, all tests pass, all benchmarks run.

- [ ] **Step 6: Commit Week 12**

```bash
git add README.md notes/weekly/week-12.md notes/interview/final-project-summary.md notes/tradeoffs/final-tradeoffs.md missions/week-12-productization-explanation-and-evidence/README.md benchmarks/containers/static_vector_bench.cpp benchmarks/allocators/monotonic_arena_bench.cpp benchmarks/concurrency/thread_pool_bench.cpp
git commit -m "docs: finalize week 12 mastery lab deliverables"
```

---

## Verification Checklist for the Full Plan

After all tasks are complete, run these commands in order:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DCPPML_ENABLE_ASAN=ON -DCPPML_ENABLE_UBSAN=ON
cmake --build build-asan
ctest --test-dir build-asan --output-on-failure
cmake -S . -B build-tsan -DCMAKE_BUILD_TYPE=Debug -DCPPML_ENABLE_TSAN=ON
cmake --build build-tsan
ctest --test-dir build-tsan --output-on-failure -R week08|week10|week11
./build/benchmarks/static_vector_bench --benchmark_min_time=0.01
./build/benchmarks/monotonic_arena_bench --benchmark_min_time=0.01
./build/benchmarks/thread_pool_bench --benchmark_min_time=0.01
```

Expected final state:
- all build targets succeed
- all tests succeed
- sanitizer targets succeed where supported
- all three benchmark programs run
- all 12 mission files exist
- all 12 weekly notes exist
- all 4 glossary checkpoints exist
- README explains the whole lab clearly

---

Plan complete and saved to `docs/superpowers/plans/2026-04-08-cpp-engineering-mastery-lab-implementation-plan.md`. Ready to execute?
