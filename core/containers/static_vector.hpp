#pragma once

#include <cstddef>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace cppml {

template <class T, std::size_t Capacity>
class static_vector {
public:
    static_vector() = default;

    static_vector(const static_vector&) = delete;
    static_vector& operator=(const static_vector&) = delete;

    static_vector(static_vector&& other) {
        move_from(std::move(other));
    }

    static_vector& operator=(static_vector&& other) {
        if (this != &other) {
            clear();
            move_from(std::move(other));
        }

        return *this;
    }

    ~static_vector() { clear(); }

    void push_back(const T& value) {
        ensure_capacity();
        std::construct_at(raw_slot(size_), value);
        ++size_;
    }

    void push_back(T&& value) {
        ensure_capacity();
        std::construct_at(raw_slot(size_), std::move(value));
        ++size_;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }

    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Capacity; }

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

    [[nodiscard]] T& operator[](std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("static_vector index out of range");
        }

        return *live_at(index);
    }

    [[nodiscard]] const T& operator[](std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("static_vector index out of range");
        }

        return *live_at(index);
    }

    void clear() noexcept {
        while (size_ > 0) {
            --size_;
            std::destroy_at(live_at(size_));
        }
    }

private:
    using storage_type = std::aligned_storage_t<sizeof(T), alignof(T)>;

    [[nodiscard]] T* raw_slot(std::size_t index) noexcept {
        return reinterpret_cast<T*>(&storage_[index]);
    }

    [[nodiscard]] const T* raw_slot(std::size_t index) const noexcept {
        return reinterpret_cast<const T*>(&storage_[index]);
    }

    [[nodiscard]] T* live_at(std::size_t index) noexcept {
        return std::launder(raw_slot(index));
    }

    [[nodiscard]] const T* live_at(std::size_t index) const noexcept {
        return std::launder(raw_slot(index));
    }

    void ensure_capacity() const {
        if (size_ >= Capacity) {
            throw std::length_error("static_vector full");
        }
    }

    void move_from(static_vector&& other) {
        try {
            for (std::size_t index = 0; index < other.size_; ++index) {
                std::construct_at(raw_slot(index), std::move(*other.live_at(index)));
                ++size_;
            }
        } catch (...) {
            clear();
            throw;
        }

        other.clear();
    }

    storage_type storage_[Capacity];
    std::size_t size_ = 0;
};

}
