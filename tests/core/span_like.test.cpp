#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

#include "core/utility/span_like.hpp"

TEST_CASE("span_like exposes size and element access", "[week01][span]") {
    int values[] = {1, 2, 3};
    cppml::span_like<int> span(values, 3);

    REQUIRE(span.size() == 3);
    REQUIRE(span[0] == 1);
    REQUIRE(span[2] == 3);
}

TEST_CASE("span_like throws on out-of-bounds access", "[week01][span]") {
    int values[] = {1, 2, 3};
    cppml::span_like<int> span(values, 3);

    REQUIRE_THROWS_AS(span[3], std::out_of_range);
}
