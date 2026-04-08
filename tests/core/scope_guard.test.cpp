#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

#include "core/memory/scope_guard.hpp"

TEST_CASE("scope_guard runs cleanup on scope exit", "[week02][scope_guard]") {
    bool cleaned = false;

    {
        cppml::scope_guard guard([&] { cleaned = true; });
        REQUIRE_FALSE(cleaned);
    }

    REQUIRE(cleaned);
}

TEST_CASE("moved-from scope_guard does not clean up twice", "[week02][scope_guard]") {
    int cleanups = 0;

    {
        auto first = cppml::scope_guard([&] { ++cleanups; });
        auto second = std::move(first);

        REQUIRE(cleanups == 0);
    }

    REQUIRE(cleanups == 1);
}

TEST_CASE("scope_guard cleans up when scope exits via exception", "[week02][scope_guard]") {
    bool cleaned = false;

    try {
        cppml::scope_guard guard([&] { cleaned = true; });
        throw std::runtime_error("boom");
    } catch (const std::runtime_error&) {
    }

    REQUIRE(cleaned);
}

TEST_CASE("scope_guard release prevents cleanup", "[week02][scope_guard]") {
    bool cleaned = false;

    {
        cppml::scope_guard guard([&] { cleaned = true; });
        guard.release();
    }

    REQUIRE_FALSE(cleaned);
}
