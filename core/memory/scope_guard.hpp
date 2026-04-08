#pragma once

#include <type_traits>
#include <utility>

namespace cppml {

template <class F>
class scope_guard {
public:
    explicit scope_guard(F fn) noexcept(std::is_nothrow_move_constructible_v<F>)
        : fn_(std::move(fn)) {}

    scope_guard(const scope_guard&) = delete;
    scope_guard& operator=(const scope_guard&) = delete;

    scope_guard(scope_guard&& other) noexcept(std::is_nothrow_move_constructible_v<F>)
        : fn_(std::move(other.fn_)), active_(std::exchange(other.active_, false)) {}

    scope_guard& operator=(scope_guard&&) = delete;

    ~scope_guard() noexcept {
        if (!active_) {
            return;
        }

        try {
            fn_();
        } catch (...) {
        }
    }

    void release() noexcept { active_ = false; }

private:
    F fn_;
    bool active_ = true;
};

template <class F>
scope_guard(F) -> scope_guard<F>;

}
