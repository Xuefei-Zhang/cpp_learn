#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

#include "core/memory/unique_resource.hpp"

TEST_CASE("unique_resource releases exactly once", "[week02][unique_resource]") {
    int releases = 0;

    {
        auto deleter = [&](int&) { ++releases; };
        cppml::unique_resource<int, decltype(deleter)> resource(7, deleter);

        REQUIRE(resource.get() == 7);
    }

    REQUIRE(releases == 1);
}

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

TEST_CASE("moved-from unique_resource does not release twice", "[week02][unique_resource]") {
    int releases = 0;

    {
        auto deleter = [&](int&) { ++releases; };
        auto first = cppml::unique_resource<int, decltype(deleter)>(7, deleter);
        auto second = std::move(first);

        REQUIRE(second.get() == 7);
    }

    REQUIRE(releases == 1);
}

TEST_CASE("unique_resource does not retry cleanup after throwing reset", "[week02][unique_resource]") {
    int attempts = 0;

    {
        auto deleter = [&](int&) {
            ++attempts;
            throw std::runtime_error("boom");
        };

        cppml::unique_resource<int, decltype(deleter)> resource(7, deleter);

        REQUIRE_THROWS_AS(resource.reset(), std::runtime_error);
        REQUIRE(attempts == 1);
    }

    REQUIRE(attempts == 1);
}
