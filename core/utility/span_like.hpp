#pragma once

#include <cstddef>
#include <stdexcept>

namespace cppml {

template <class T>
class span_like {
public:
    using value_type = T;

    constexpr span_like(T* data, std::size_t size) noexcept : data_(data), size_(size) {}

    [[nodiscard]] constexpr std::size_t size() const noexcept { return size_; }

    [[nodiscard]] constexpr T& operator[](std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("span_like index out of range");
        }

        return data_[index];
    }

private:
    T* data_;
    std::size_t size_;
};

}
