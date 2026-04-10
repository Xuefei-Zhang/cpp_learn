#pragma once

// std::size_t and std::byte-related layout helpers depend on this header.
#include <cstddef>
// construct_at, destroy_at, and aligned_storage_t live here.
#include <memory>
// Low-level object lifetime utilities such as std::launder are declared here.
#include <new>
// We use standard exception types for clear failure modes.
#include <stdexcept>
// Used for tiny compile-time type properties.
#include <type_traits>
// std::move is essential for value transfer in push_back and move_from.
#include <utility>

namespace cppml {

// static_vector is a deliberately restricted container for learning:
// - storage is fixed at compile time
// - no reallocation ever happens
// - object lifetime is managed manually inside raw storage
template <class T, std::size_t Capacity>
class static_vector {
public:
    // Default construction does not construct any T objects yet.
    // It only creates the container object whose storage is currently empty.
    static_vector() = default;

    // Copying is disabled to keep Week 3 focused on lifetime mechanics,
    // move support, and manual destruction rather than copy semantics.
    static_vector(const static_vector&) = delete;
    static_vector& operator=(const static_vector&) = delete;

    // Move construction delegates to move_from, which manually constructs each
    // destination element from the source container's live elements.
    static_vector(static_vector&& other) {
        move_from(std::move(other));
    }

    // Move assignment first destroys this container's current live elements,
    // then repopulates it from the source container.
    static_vector& operator=(static_vector&& other) {
        if (this != &other) {
            clear();
            move_from(std::move(other));
        }

        return *this;
    }

    // Destruction simply means: destroy however many live elements currently exist.
    ~static_vector() { clear(); }

    // Copy-inserting an lvalue element.
    // The important sequence is: check capacity -> construct new element -> increment size.
    // Incrementing after construction preserves a stronger invariant if construction throws.
    void push_back(const T& value) {
        ensure_capacity();
        std::construct_at(raw_slot(size_), value);
        ++size_;
    }

    // Move-inserting an rvalue element follows the same invariant-preserving order.
    void push_back(T&& value) {
        ensure_capacity();
        std::construct_at(raw_slot(size_), std::move(value));
        ++size_;
    }

    // Number of currently live elements, not total raw storage slots.
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

    // Fixed capacity is known at compile time, so this is just the template parameter.
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Capacity; }

    // Empty means there are zero live elements.
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

    // Checked element access for the learning lab.
    // std::vector::operator[] is unchecked, but here we prefer explicit runtime
    // feedback because the point is understanding invariants, not mimicking STL exactly.
    [[nodiscard]] T& operator[](std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("static_vector index out of range");
        }

        // Once we know the index refers to a live element, we can safely view
        // the storage slot as a real T object.
        return *live_at(index);
    }

    [[nodiscard]] const T& operator[](std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("static_vector index out of range");
        }

        return *live_at(index);
    }

    // Destroy all currently live elements in reverse order.
    // Reverse destruction matches the usual stack-like lifetime intuition and
    // avoids touching storage that never held a live object.
    void clear() noexcept {
        while (size_ > 0) {
            --size_;
            std::destroy_at(live_at(size_));
        }
    }

private:
    // Each array slot has the right size and alignment for T, but does not by
    // itself mean a T object exists there yet.
    using storage_type = std::aligned_storage_t<sizeof(T), alignof(T)>;

    // raw_slot gives us a pointer to storage only.
    // This is what we use before object lifetime begins, for example with construct_at.
    [[nodiscard]] T* raw_slot(std::size_t index) noexcept {
        return reinterpret_cast<T*>(&storage_[index]);
    }

    [[nodiscard]] const T* raw_slot(std::size_t index) const noexcept {
        return reinterpret_cast<const T*>(&storage_[index]);
    }

    // live_at is for slots that already contain a live T object.
    // std::launder is used here because we are reinterpreting storage that has
    // had object lifetime started within it.
    [[nodiscard]] T* live_at(std::size_t index) noexcept {
        return std::launder(raw_slot(index));
    }

    [[nodiscard]] const T* live_at(std::size_t index) const noexcept {
        return std::launder(raw_slot(index));
    }

    // Fixed-capacity overflow is one of the key explicit failure modes of this type.
    void ensure_capacity() const {
        if (size_ >= Capacity) {
            throw std::length_error("static_vector full");
        }
    }

    // move_from is the heart of the manual-lifetime lesson.
    // It constructs destination elements one by one from the source.
    // If one move throws, we must destroy the already-constructed destination
    // prefix before rethrowing, or we leak live objects.
    void move_from(static_vector&& other) {
        try {
            for (std::size_t index = 0; index < other.size_; ++index) {
                std::construct_at(raw_slot(index), std::move(*other.live_at(index)));
                // Track each successfully constructed element immediately so clear()
                // knows exactly how many live objects exist if a later move throws.
                ++size_;
            }
        } catch (...) {
            // Roll back the partially-built destination and then propagate the error.
            clear();
            throw;
        }

        // Once every destination element exists, the source container can destroy
        // its old live elements and become empty.
        other.clear();
    }

    // Fixed-size raw storage buffer.
    storage_type storage_[Capacity];
    // Number of live elements currently constructed inside storage_.
    std::size_t size_ = 0;
};

}
