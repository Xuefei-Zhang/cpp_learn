#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>

#include "core/containers/static_vector.hpp"

namespace {

struct counted_element {
    static inline int destructions = 0;

    int value = 0;

    counted_element() = default;

    explicit counted_element(int v) : value(v) {}

    counted_element(const counted_element&) = default;
    counted_element(counted_element&&) noexcept = default;
    counted_element& operator=(const counted_element&) = default;
    counted_element& operator=(counted_element&&) noexcept = default;

    ~counted_element() { ++destructions; }
};

struct throwing_move_element {
    static inline int alive = 0;
    static inline int move_attempts = 0;
    static inline int throw_on_move_number = 0;

    int value = 0;

    explicit throwing_move_element(int v) : value(v) { ++alive; }

    throwing_move_element(const throwing_move_element& other) : value(other.value) { ++alive; }

    throwing_move_element(throwing_move_element&& other) : value(other.value) {
        ++move_attempts;
        if (throw_on_move_number != 0 && move_attempts == throw_on_move_number) {
            throw std::runtime_error("boom");
        }

        ++alive;
        other.value = -1;
    }

    throwing_move_element& operator=(const throwing_move_element&) = default;
    throwing_move_element& operator=(throwing_move_element&&) = default;

    ~throwing_move_element() { --alive; }
};

}

TEST_CASE("static_vector pushes and indexes values", "[week03][static_vector]") {
    cppml::static_vector<int, 4> values;

    values.push_back(1);
    values.push_back(2);

    REQUIRE(values.size() == 2);
    REQUIRE(values[0] == 1);
    REQUIRE(values[1] == 2);
}

TEST_CASE("static_vector throws when capacity is exceeded", "[week03][static_vector]") {
    cppml::static_vector<int, 2> values;

    values.push_back(1);
    values.push_back(2);

    REQUIRE_THROWS_AS(values.push_back(3), std::length_error);
}

TEST_CASE("static_vector throws on out-of-range indexing", "[week03][static_vector]") {
    cppml::static_vector<int, 2> values;
    values.push_back(1);

    REQUIRE_THROWS_AS(values[1], std::out_of_range);
}

TEST_CASE("static_vector clear destroys contained elements", "[week03][static_vector]") {
    counted_element::destructions = 0;

    cppml::static_vector<counted_element, 4> values;
    values.push_back(counted_element{1});
    values.push_back(counted_element{2});

    counted_element::destructions = 0;
    values.clear();

    REQUIRE(values.size() == 0);
    REQUIRE(counted_element::destructions == 2);
}

TEST_CASE("static_vector supports move-only elements", "[week03][static_vector]") {
    cppml::static_vector<std::unique_ptr<int>, 2> values;

    values.push_back(std::make_unique<int>(7));

    REQUIRE(values.size() == 1);
    REQUIRE(*values[0] == 7);
}

TEST_CASE("static_vector move construction transfers elements", "[week03][static_vector]") {
    cppml::static_vector<int, 4> source;
    source.push_back(1);
    source.push_back(2);

    cppml::static_vector<int, 4> moved(std::move(source));

    REQUIRE(moved.size() == 2);
    REQUIRE(moved[0] == 1);
    REQUIRE(moved[1] == 2);
    REQUIRE(source.empty());
}

TEST_CASE("static_vector move assignment transfers elements", "[week03][static_vector]") {
    cppml::static_vector<int, 4> source;
    source.push_back(3);
    source.push_back(4);

    cppml::static_vector<int, 4> destination;
    destination.push_back(9);

    destination = std::move(source);

    REQUIRE(destination.size() == 2);
    REQUIRE(destination[0] == 3);
    REQUIRE(destination[1] == 4);
    REQUIRE(source.empty());
}

TEST_CASE("static_vector move construction cleans up partially built destination on throw", "[week03][static_vector]") {
    throwing_move_element::alive = 0;
    throwing_move_element::move_attempts = 0;
    throwing_move_element::throw_on_move_number = 0;

    {
        cppml::static_vector<throwing_move_element, 4> source;
        throwing_move_element first(1);
        throwing_move_element second(2);

        source.push_back(first);
        source.push_back(second);

        REQUIRE(throwing_move_element::alive == 4);

        throwing_move_element::move_attempts = 0;
        throwing_move_element::throw_on_move_number = 2;

        auto attempt_move = [&] {
            cppml::static_vector<throwing_move_element, 4> moved(std::move(source));
        };

        REQUIRE_THROWS_AS(attempt_move(), std::runtime_error);
    }

    REQUIRE(throwing_move_element::alive == 0);
    throwing_move_element::throw_on_move_number = 0;
}
