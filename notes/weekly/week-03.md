# Week 3 Notes

`static_vector` makes object lifetime visible in a way ordinary `std::vector` use often hides. The container owns a fixed amount of raw storage up front, but the elements only begin life when `push_back` constructs them in place. That split between allocated storage and live objects is the key lesson this week.

Because capacity is fixed, this slice avoids reallocation entirely and keeps the scope centered on construction, destruction, and bounds decisions. `push_back` either constructs one new element and increments the size or throws `std::length_error` before changing anything, which gives a simple strong guarantee for overflow in this minimal design.

`clear()` matters as much as insertion. The container has to destroy only the elements that are currently alive, in reverse order, and then leave the raw storage available for reuse. That is the practical difference between “bytes exist” and “objects exist” in C++.

The move-only test is also useful even in this restricted version. If the container can hold `std::unique_ptr<int>`, it proves the API is not secretly relying on copyability and that ownership-aware element types still work with placement construction.
