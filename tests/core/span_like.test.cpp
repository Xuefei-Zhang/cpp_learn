#include <catch2/catch_test_macros.hpp>

// We need std::out_of_range for the negative test.
#include <stdexcept>

// Tests include the production header exactly the way a consumer would.
#include "core/utility/span_like.hpp"

// This first test is the happy-path contract:
// a span-like view should report its size and let us read valid elements.
TEST_CASE("span_like exposes size and element access", "[week01][span]") {
    // Use a plain C array so the borrowed-storage story stays obvious.
    int values[] = {1, 2, 3};
    cppml::span_like<int> span(values, 3);

    // size() should reflect the logical element count we passed in.
    REQUIRE(span.size() == 3);
    // Valid indexes should return references into the original storage.
    REQUIRE(span[0] == 1);
    REQUIRE(span[2] == 3);
}

// This test pins down the teaching choice that operator[] is checked in this lab.
TEST_CASE("span_like throws on out-of-bounds access", "[week01][span]") {
    int values[] = {1, 2, 3};
    cppml::span_like<int> span(values, 3);

    // Index 3 is just past the logical end, so the access must fail explicitly.
    REQUIRE_THROWS_AS(span[3], std::out_of_range);
}
