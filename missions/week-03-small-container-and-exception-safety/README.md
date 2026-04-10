# Week 3 Mission: Small Container and Exception Safety

## Why

This week turns lifetime management into container mechanics. The goal is to understand how raw storage becomes live objects, how those objects are destroyed safely, and how even a tiny container has to make clear promises when operations fail.

## Build

Implement `cppml::static_vector<T, Capacity>` in `core/containers/static_vector.hpp` as a deliberately restricted fixed-capacity container. Keep it small: inline storage, `push_back`, checked indexing, `size()`, `clear()`, and the minimum move support needed for ownership-aware element types.

## Tests

Add Week 3 tests that prove `static_vector` stores pushed values, reports size correctly, throws `std::length_error` when capacity is exceeded, destroys live elements on `clear()`, and can hold a move-only element such as `std::unique_ptr<int>` without expanding into a general-purpose vector clone.

## Benchmarks

Replace the placeholder container benchmark with a first real measurement that repeatedly constructs a `static_vector<int, 1024>`, pushes a fixed number of integers, and reports a timing row locally. The benchmark exists to start performance conversations, not to claim the container is fully optimized.

## Notes

Record the difference between reserved storage and constructed objects, why checked overflow is part of the learning goal here, what guarantee `push_back` provides when the container is already full, why `clear()` must destroy only live elements, and how move-only support shows the API respects ownership-sensitive types.

## Do Not Do

Do not add allocator support, dynamic growth, iterators, `emplace_back`, or any Week 4+ generic API work. Do not turn this slice into a full `std::vector` replacement.
