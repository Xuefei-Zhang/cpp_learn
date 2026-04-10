#pragma once

// We use type traits to express noexcept conditions based on the callable type.
#include <type_traits>
// Utility helpers such as std::move and std::exchange live here.
#include <utility>

namespace cppml {

// scope_guard owns exactly one cleanup action.
// When the guard dies, it runs that action unless the guard was released.
template <class F>
class scope_guard {
public:
    // Store the cleanup callable by value inside the guard.
    // The constructor is conditionally noexcept: it is noexcept only if moving
    // the callable into the guard is itself noexcept.
    explicit scope_guard(F fn) noexcept(std::is_nothrow_move_constructible_v<F>)
        : fn_(std::move(fn)) {}

    // Copying is disabled because two guards owning the same cleanup action
    // would make exactly-once cleanup much harder to reason about.
    scope_guard(const scope_guard&) = delete;
    scope_guard& operator=(const scope_guard&) = delete;

    // Moving transfers the cleanup responsibility to the destination guard.
    // std::exchange both reads the old `active_` value and turns the source off.
    scope_guard(scope_guard&& other) noexcept(std::is_nothrow_move_constructible_v<F>)
        : fn_(std::move(other.fn_)), active_(std::exchange(other.active_, false)) {}

    // Move assignment is omitted in this small teaching type.
    // That keeps the ownership model compact for Week 2.
    scope_guard& operator=(scope_guard&&) = delete;

    // Destructors should not let exceptions escape here.
    // If cleanup code throws during stack unwinding, the program can terminate.
    ~scope_guard() noexcept {
        // If the guard has been released or moved from, it owns no cleanup work.
        if (!active_) {
            return;
        }

        // Run the cleanup function, but swallow any exception so the destructor
        // keeps its noexcept contract.
        try {
            fn_();
        } catch (...) {
        }
    }

    // Explicitly disarm the guard so destruction no longer runs the cleanup.
    void release() noexcept { active_ = false; }

private:
    // The cleanup callable itself.
    F fn_;
    // Tracks whether this particular guard instance still owns cleanup duty.
    bool active_ = true;
};

// Class template argument deduction lets us write `scope_guard guard(lambda);`
// without spelling out the lambda type manually.
template <class F>
scope_guard(F) -> scope_guard<F>;

}
