# Week 1 Notes

`span_like` is intentionally non-owning: it stores a pointer and a size, but it does not allocate memory, free memory, or extend the lifetime of the referenced elements. That makes it cheap to copy and good for expressing “borrowed contiguous data” in APIs.

This lab also makes `span_like::operator[]` bounds-checked and exception-throwing. That is deliberate for learning: the checked access keeps ownership and API-boundary mistakes visible while we are still building intuition about views, but it is not meant to reproduce `std::span` behavior exactly.

That same design leaves important bugs outside its protection boundary. `span_like` cannot stop use-after-free if the original array dies too early, it cannot prove the pointer and size actually describe valid contiguous storage, and it cannot prevent concurrent mutation problems if another owner changes the data unexpectedly.

The useful lesson for Week 1 is that a non-owning abstraction can improve API clarity without pretending to be full memory safety. The caller still owns lifetime correctness.
