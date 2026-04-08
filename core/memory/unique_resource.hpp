#pragma once

#include <type_traits>
#include <utility>

namespace cppml {

template <class R, class D>
class unique_resource {
public:
    unique_resource(R resource, D deleter) noexcept(
        std::is_nothrow_move_constructible_v<R> && std::is_nothrow_move_constructible_v<D>)
        : resource_(std::move(resource)), deleter_(std::move(deleter)) {}

    unique_resource(const unique_resource&) = delete;
    unique_resource& operator=(const unique_resource&) = delete;

    unique_resource(unique_resource&& other) noexcept(
        std::is_nothrow_move_constructible_v<R> && std::is_nothrow_move_constructible_v<D>)
        : resource_(std::move(other.resource_)), deleter_(std::move(other.deleter_)),
          active_(std::exchange(other.active_, false)) {}

    unique_resource& operator=(unique_resource&&) = delete;

    ~unique_resource() noexcept {
        try {
            reset();
        } catch (...) {
        }
    }

    [[nodiscard]] R& get() noexcept { return resource_; }
    [[nodiscard]] const R& get() const noexcept { return resource_; }

    void reset() {
        if (active_) {
            active_ = false;
            deleter_(resource_);
        }
    }

private:
    R resource_;
    D deleter_;
    bool active_ = true;
};

}
