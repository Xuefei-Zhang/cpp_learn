#pragma once

// std::size_t is the standard unsigned type used for sizes and indexes.
#include <cstddef>
// We throw std::out_of_range when the caller indexes past the logical end.
#include <stdexcept>

// Keep project symbols in their own namespace instead of the global namespace.
namespace cppml {

// This is a deliberately tiny teaching version of a span-like type.
// It does NOT own memory. It only remembers where some existing elements live
// and how many of them should be considered part of the view.
template <class T>
class span_like {
public:
    // `value_type` matches a common standard-library convention and makes the
    // type easier to talk about generically.
    using value_type = T;

    // The constructor simply stores a pointer and a size.
    // `noexcept` is valid because assigning these two data members cannot throw.
    constexpr span_like(T* data, std::size_t size) noexcept : data_(data), size_(size) {}

    // [[nodiscard]] tells callers that ignoring the return value is probably a mistake.
    // This is especially reasonable for metadata queries like size().
    [[nodiscard]] constexpr std::size_t size() const noexcept { return size_; }

    // This operator returns a reference into the viewed storage.
    // In this lab we intentionally perform bounds checking here so the learner
    // can see that "non-owning" and "unchecked" are separate design choices.
    [[nodiscard]] constexpr T& operator[](std::size_t index) const {
        // If the index is outside the logical range, we fail explicitly.
        if (index >= size_) {
            throw std::out_of_range("span_like index out of range");
        }

        // Because the span is non-owning, this simply forwards access into the
        // caller-provided storage. If the original storage is dead, this code
        // cannot save us; that lifetime rule belongs to the caller.
        return data_[index];
    }

private:
    // Pointer to the first element of somebody else's storage.
    T* data_;
    // Number of elements this view is allowed to expose.
    std::size_t size_;
};

}
