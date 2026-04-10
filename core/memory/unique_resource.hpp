#pragma once

// Type traits let us express noexcept based on the resource/deleter types.
#include <type_traits>
// Utility helpers such as std::move and std::exchange are defined here.
#include <utility>

namespace cppml {

// unique_resource is a generic RAII wrapper:
// it owns a resource value and a custom deleter that knows how to release it.
template <class R, class D>
class unique_resource {
public:
    // Construct the owned resource and deleter in place inside the wrapper.
    // The constructor is conditionally noexcept because both subobjects are moved in.
    unique_resource(R resource, D deleter) noexcept(
        std::is_nothrow_move_constructible_v<R> && std::is_nothrow_move_constructible_v<D>)
        : resource_(std::move(resource)), deleter_(std::move(deleter)) {}

    // Copying is disabled because this wrapper models unique ownership.
    unique_resource(const unique_resource&) = delete;
    unique_resource& operator=(const unique_resource&) = delete;

    // Move construction transfers both the resource and the responsibility to
    // release it. The source is marked inactive so it will not clean up twice.
    unique_resource(unique_resource&& other) noexcept(
        std::is_nothrow_move_constructible_v<R> && std::is_nothrow_move_constructible_v<D>)
        : resource_(std::move(other.resource_)), deleter_(std::move(other.deleter_)),
          active_(std::exchange(other.active_, false)) {}

    // Move assignment is skipped in this teaching version to keep the exact
    // ownership rules easy to track.
    unique_resource& operator=(unique_resource&&) = delete;

    // Destruction attempts cleanup automatically.
    // We intentionally swallow cleanup exceptions here so the destructor stays
    // noexcept and can be safely used during unwinding.
    ~unique_resource() noexcept {
        try {
            reset();
        } catch (...) {
        }
    }

    // Expose the owned resource by reference without transferring ownership.
    [[nodiscard]] R& get() noexcept { return resource_; }
    [[nodiscard]] const R& get() const noexcept { return resource_; }

    // reset() is the explicit "release now" operation.
    // We turn `active_` off before calling the deleter so even if the deleter
    // throws, the wrapper will not try cleanup a second time later.
    void reset() {
        if (active_) {
            active_ = false;
            deleter_(resource_);
        }
    }

private:
    // The actual resource we own, such as an int handle in these small tests.
    R resource_;
    // The policy object that knows how to release that resource.
    D deleter_;
    // Tracks whether this wrapper still owns one cleanup attempt.
    bool active_ = true;
};

}
