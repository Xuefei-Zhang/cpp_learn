#include <catch2/catch_test_macros.hpp>

// Needed for the throwing-deleter scenario.
#include <stdexcept>

#include "core/memory/unique_resource.hpp"

// Basic RAII contract: if the wrapper stays active until destruction,
// its deleter should run exactly once.
TEST_CASE("unique_resource releases exactly once", "[week02][unique_resource]") {
    int releases = 0;

    {
        // The deleter increments a counter so we can observe cleanup.
        auto deleter = [&](int&) { ++releases; };
        cppml::unique_resource<int, decltype(deleter)> resource(7, deleter);

        // get() exposes the owned resource without transferring ownership.
        REQUIRE(resource.get() == 7);
    }

    REQUIRE(releases == 1);
}

// reset() is the manual "release now" operation.
// Calling it more than once should not cause repeated cleanup attempts.
TEST_CASE("unique_resource reset is idempotent", "[week02][unique_resource]") {
    int releases = 0;

    {
        auto deleter = [&](int&) { ++releases; };
        cppml::unique_resource<int, decltype(deleter)> resource(7, deleter);

        resource.reset();
        resource.reset();

        REQUIRE(releases == 1);
    }

    REQUIRE(releases == 1);
}

// Moving unique_resource should transfer cleanup responsibility to the destination.
TEST_CASE("moved-from unique_resource does not release twice", "[week02][unique_resource]") {
    int releases = 0;

    {
        auto deleter = [&](int&) { ++releases; };
        auto first = cppml::unique_resource<int, decltype(deleter)>(7, deleter);
        // second becomes the active owner.
        auto second = std::move(first);

        REQUIRE(second.get() == 7);
    }

    REQUIRE(releases == 1);
}

// This test pins down an important subtlety:
// if reset() throws, the wrapper still must not attempt cleanup again later.
TEST_CASE("unique_resource does not retry cleanup after throwing reset", "[week02][unique_resource]") {
    int attempts = 0;

    {
        auto deleter = [&](int&) {
            ++attempts;
            throw std::runtime_error("boom");
        };

        cppml::unique_resource<int, decltype(deleter)> resource(7, deleter);

        // The first reset throws, but it still counts as the one cleanup attempt.
        REQUIRE_THROWS_AS(resource.reset(), std::runtime_error);
        REQUIRE(attempts == 1);
    }

    // Destruction must not retry the deleter after that throwing reset.
    REQUIRE(attempts == 1);
}
