#include <catch2/catch_test_macros.hpp>

// Used for the exception-path cleanup test.
#include <stdexcept>

#include "core/memory/scope_guard.hpp"

// Happy-path contract: if the guard remains active until scope exit,
// it must run its cleanup callable exactly once.
TEST_CASE("scope_guard runs cleanup on scope exit", "[week02][scope_guard]") {
    bool cleaned = false;

    // Create an inner scope so we can observe destruction at a precise point.
    {
        cppml::scope_guard guard([&] { cleaned = true; });
        REQUIRE_FALSE(cleaned);
    }

    // The cleanup should have happened when the inner scope ended.
    REQUIRE(cleaned);
}

// Moving a scope_guard transfers cleanup responsibility.
// The moved-from guard must not also run the same cleanup.
TEST_CASE("moved-from scope_guard does not clean up twice", "[week02][scope_guard]") {
    int cleanups = 0;

    {
        auto first = cppml::scope_guard([&] { ++cleanups; });
        // second now owns the cleanup action; first should become inactive.
        auto second = std::move(first);

        REQUIRE(cleanups == 0);
    }

    // Exactly one guard should have fired.
    REQUIRE(cleanups == 1);
}

// RAII matters most on non-happy paths.
// This test proves the cleanup still happens when the scope exits by exception.
TEST_CASE("scope_guard cleans up when scope exits via exception", "[week02][scope_guard]") {
    bool cleaned = false;

    try {
        cppml::scope_guard guard([&] { cleaned = true; });
        throw std::runtime_error("boom");
    } catch (const std::runtime_error&) {
    }

    REQUIRE(cleaned);
}

// release() is the explicit way to say: "this guard no longer owns cleanup".
TEST_CASE("scope_guard release prevents cleanup", "[week02][scope_guard]") {
    bool cleaned = false;

    {
        cppml::scope_guard guard([&] { cleaned = true; });
        // Disarm before leaving scope.
        guard.release();
    }

    // Because the guard was released, destruction should do nothing.
    REQUIRE_FALSE(cleaned);
}
