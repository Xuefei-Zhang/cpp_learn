# C++ Engineering Mastery Lab

This repository is a 12-week C++20 practice lab focused on modern engineering habits: ownership, value semantics, RAII, testing, benchmarking, and explainable tradeoffs. The codebase grows week by week into a small task runtime and resource-aware execution toolkit instead of a pile of disconnected demos.

## Week-by-week structure

- Weeks 1-3: bootstrap, value semantics, RAII, and lifetime-aware container work
- Weeks 4-6: templates, constraints, callable abstractions, and range-oriented design
- Weeks 7-9: allocation strategy, concurrency, sanitizers, and exception-safety hardening
- Weeks 10-12: task runtime assembly, resource-aware integration, and final productization

## Current Week 1 slice

Week 1 boots the repository, wires CMake + tests + benchmarks, and introduces `cppml::span_like<T>` as a minimal non-owning view over contiguous storage. In this lab, `span_like::operator[]` performs bounds checking and throws `std::out_of_range` on invalid access; that is an intentional learning-oriented choice for Week 1, not an attempt to mirror `std::span` exactly.

## Build and test locally

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure -R week01
```

## Repository focus areas

- `core/`: reusable hand-built C++ modules
- `runtime/`: runtime and systems-oriented support code added in later weeks
- `systems/`: integrated mini-systems built from earlier modules
- `tests/`: verification for each weekly slice
- `benchmarks/`: simple measurements to keep performance claims honest
- `notes/`, `docs/`, `missions/`: learning artifacts, vocabulary checkpoints, and scoped weekly goals
