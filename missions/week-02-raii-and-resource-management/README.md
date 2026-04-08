# Week 2 Mission: RAII and Resource Management

## Why

This week makes lifetime management explicit so cleanup happens reliably on both normal and exceptional exits. The goal is to build intuition that ownership is a semantic boundary encoded in types, not a comment or a convention.

## Build

Implement `cppml::scope_guard` in `core/memory/scope_guard.hpp` and `cppml::unique_resource` in `core/memory/unique_resource.hpp`. Keep both types minimal and move-only: one stores a scope-exit cleanup action, the other stores a resource and the deleter that releases it.

## Tests

Add Week 2 tests that prove `scope_guard` runs cleanup on scope exit, `release()` disables cleanup, moved-from guards do not clean up twice, cleanup still happens when the guarded scope exits through an exception, `unique_resource` releases exactly once, moved-from resources do not release twice, `reset()` is idempotent, and a throwing deleter is not retried by later reset or destruction.

## Benchmarks

Do not add new benchmarks this week. The benchmark work stays in the already wired repository targets for later weeks.

## Notes

Record why move-only ownership matters here, why destructor-based cleanup helps on failure paths, why `reset()` is safe to call more than once in this design, why destructors remain `noexcept` and swallow cleanup exceptions in this minimal foundation, and where these tiny helpers intentionally stop short of being full standard-library replacements.

## Do Not Do

Do not add file-handle wrappers, policy-heavy cleanup helpers, copy semantics, allocator support, or any Week 3+ container behavior.