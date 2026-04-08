# Week 1 Mission: Project Bootstrap and Value Semantics

## Why

This week establishes the repository shape and the first ownership-focused utility so later weeks can build on a real codebase instead of isolated snippets. The goal is to make non-owning design explicit from the start.

## Build

Implement the minimal CMake-based project setup, test wiring, benchmark wiring, and `cppml::span_like<T>` in `core/utility/span_like.hpp`. Keep `span_like` small: pointer plus size, read access, and bounds checking only, with the checked `operator[]` treated as a learning-oriented deviation rather than a promise to match `std::span`.

## Tests

Add Week 1 tests that prove `span_like` reports its size, returns elements by index for a simple array, and throws `std::out_of_range` when indexed past the end.

## Benchmarks

Wire a minimal benchmark executable so the repository can already build a benchmark target locally. Do not optimize anything yet; this week is only about build-system readiness.

## Notes

Record the difference between owning and non-owning interfaces, why `span_like` is cheap to pass around, why this lab uses a checked `operator[]` for teaching purposes, and which classes of bugs it still cannot prevent.

## Do Not Do

Do not add copy or move extras beyond the defaults, do not add iterators or slicing helpers, do not generalize into a full `std::span` replacement, and do not implement any Week 2+ modules.
