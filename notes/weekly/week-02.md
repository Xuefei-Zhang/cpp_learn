# Week 2 Notes

This week turns constructors and destructors into explicit ownership boundaries instead of treating them as syntax trivia. `scope_guard` owns one cleanup action for one lexical scope, while `unique_resource` owns one resource plus the rule for releasing it.

The important behavior is on failure paths. A moved-from guard or resource must become inactive so destruction does not clean up twice, and cleanup still has to happen when control leaves the scope because of an exception.

`unique_resource::reset()` is intentionally idempotent in this slice. It disarms cleanup before invoking the deleter, so even a throwing deleter is attempted at most once. That keeps manual early release safe and prevents destructor-time retry after a failed reset.

The main lesson is that RAII is about making the safe path automatic. Once the ownership boundary is encoded in the type, exceptional exits and normal exits follow the same cleanup rule, and the destructor itself should remain a stable `noexcept` boundary instead of making later unwinding more fragile.
