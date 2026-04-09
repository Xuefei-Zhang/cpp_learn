---
title: Phase A Engineering Foundation and Object Model
tags:
  - cpp
  - phase-guide
  - modern-cpp
  - ownership
  - value-semantics
aliases:
  - Phase A Guide
  - Engineering Foundation and Object Model
---

# Phase A: Engineering Foundation and Object Model

## Why this phase exists

Phase A exists to repair a very common C++ weakness: being able to read syntax and write small programs, but not yet being able to reason clearly about **what owns what**, **when objects start and stop existing**, and **why apparently simple code becomes fragile under failure, copying, or reallocation**.

A lot of later C++ pain is not caused by templates, concurrency, or allocators. It starts much earlier:

- a view outlives the storage it points at
- a destructor runs twice because ownership was not moved correctly
- a container reallocates and invalidates references silently
- code looks fine until an exception interrupts a partially-completed operation

This phase is where you stop treating C++ as “syntax + STL” and start treating it as a language where **object lifetime, resource lifetime, and interface boundaries are first-class design decisions**.

Phase A covers Weeks 1-3:
- [[../../missions/week-01-project-bootstrap-and-value-semantics/README|Week 1 Mission]]
- [[../../missions/week-02-raii-and-resource-management/README|Week 2 Mission]]
- [[../../missions/week-03-small-container-and-exception-safety/README|Week 3 Mission]]

It also connects directly to the first glossary checkpoint:
- [[../glossary/week-01-core-terms|Week 1 Core Terms]]

## Phase goal

By the end of Phase A, you should be able to:

- explain the difference between owning and non-owning interfaces
- design small types whose lifetime behavior is intentional, not accidental
- reason about copy vs move at the type boundary
- understand what a container must do during construction, destruction, and growth
- recognize when exception safety changes the shape of an implementation

If you cannot explain those clearly yet, later phases will feel harder than they really are.

## The mental model for this phase

The shortest useful summary is:

> In C++, every abstraction is also a lifetime policy.

That sounds abstract, but it has concrete consequences.

### A type is not just data

When you define a type, you are not only choosing fields. You are choosing:

- who is responsible for cleanup
- whether the type can be copied safely
- whether it should be movable
- whether destruction can fail
- whether it is safe to borrow data from elsewhere
- whether its clients can accidentally outlive its dependencies

That is why Phase A begins with small types like `span_like`, `scope_guard`, `unique_resource`, and a restricted vector-like container. Those are not “toy examples.” They are concentrated exercises in the core problem of C++ design.

### Three distinct but related lifetimes

In this phase, keep these separate in your head:

1. **Object lifetime**  
   When a particular object begins existence and when it ends.

2. **Storage lifetime**  
   When the raw memory backing an object is available.

3. **Resource lifetime**  
   When a non-memory resource such as a file handle, lock, or OS object is acquired and released.

C++ lets those line up nicely, but only if you design deliberately.

### A good beginner trap to avoid

Many learners think:

> “If the code compiles and tests pass, the lifetime model is probably fine.”

That is false in C++. A lot of lifetime bugs are *structural*, not syntactic. They appear only when:

- something is moved
- something throws
- something grows
- something outlives a scope
- an optimization changes the call path

This is why Phase A includes both behavior tests and conceptual notes.

## Week 1 deep dive: bootstrap and value-semantics baseline

### What Week 1 is really teaching

Week 1 looks simple on the surface: set up CMake, add tests, add a benchmark target, and implement a non-owning `span_like`.

But the real lesson is not the build system. The real lesson is:

> Not every useful type should own its data.

That matters because beginners often over-own everything. They wrap storage in classes that allocate when they only needed a borrowed view, or they pass raw pointers around without making the borrowing relationship explicit.

### Owning vs non-owning interfaces

This is one of the most important distinctions in C++.

#### Owning type

An owning type is responsible for cleanup.

Examples:
- `std::string`
- `std::vector<T>`
- `std::unique_ptr<T>`
- `cppml::unique_resource`

Characteristics:
- usually controls resource lifetime directly
- often disables copying or defines copy carefully
- can often move ownership cheaply

#### Non-owning type

A non-owning type observes storage or a resource that someone else owns.

Examples:
- `std::span<T>`
- `std::string_view`
- iterator pairs
- raw pointer used as observer only
- `cppml::span_like<T>`

Characteristics:
- cheap to copy
- should not free anything
- lifetime safety depends on caller discipline

### Why `span_like` matters

`span_like` is a small type, but it forces you to confront several big ideas:

- a view can be cheap and useful without owning anything
- API clarity improves when borrowing is explicit
- bounds checking and ownership are different problems
- a safe-looking interface can still be invalid if the underlying storage is dead

### What `span_like` does **not** protect you from

A checked indexing operation is useful, but it does not solve:

- dangling storage
- aliasing confusion
- data races
- invalid size/pointer pairing supplied by the caller

This is the first place where you should learn to say:

> “This abstraction improves one class of mistakes, not all of them.”

That sentence is a mark of real engineering maturity.

### Translation units and linkage: why they matter here

At first, translation units feel like build-system trivia. They are not.

You need to understand them because C++ programs are not compiled as one giant script. They are compiled as multiple translation units and then linked.

That affects:
- header design
- inline definitions
- ODR (One Definition Rule) correctness
- symbol visibility
- template placement

For Phase A, the actionable version is simple:

- keep small reusable definitions in headers when appropriate
- understand that included headers are compiled into multiple translation units
- avoid writing code in headers casually without knowing the linkage consequences

## Week 2 deep dive: RAII and cleanup semantics

### What RAII actually means

RAII is often explained as “Resource Acquisition Is Initialization.” That phrase is correct but incomplete.

The practical meaning is:

> tie cleanup to object lifetime so the normal path and the exceptional path follow the same rule.

That is why `scope_guard` and `unique_resource` matter. They force cleanup to happen because the type exists, not because the programmer remembered to write cleanup code at every return site.

### Why destructor-based cleanup is powerful

Without RAII, every function that acquires something must manually balance that acquisition on every control-flow path.

That fails under pressure because control flow multiplies:
- happy path
- early return
- error branch
- exception path
- partially initialized state

RAII compresses all of those into one rule:

- acquire in construction / initialization
- release in destruction

### `scope_guard`

`scope_guard` is conceptually simple:
- it owns one cleanup action
- it runs it once when the scope exits
- it can be disarmed via `release()`

But even this tiny type teaches several advanced lessons.

#### Lesson 1: move changes ownership

If a `scope_guard` is moved, the moved-from object must no longer fire.

Otherwise you get double cleanup.

That is why moved-from cleanup-bearing objects must usually become inactive.

#### Lesson 2: destructor policy matters

What should happen if cleanup throws?

For a foundational helper, the best answer is usually:
- destructor is `noexcept`
- destructor swallows cleanup exceptions

Why? Because destructors running during stack unwinding must not make unwinding more fragile. A throwing destructor during unwinding can terminate the program.

This is one of the first places where “simple helper class” turns into “language runtime policy decision.”

### `unique_resource`

`unique_resource` generalizes the RAII idea:
- own a resource value
- own the deleter that releases it
- guarantee at-most-once cleanup

This looks similar to `std::unique_ptr`, but it is broader:
- the resource does not have to be memory
- the deleter can encode any release rule

### Exactly-once cleanup

Exactly-once cleanup is the central invariant here.

That means:
- moved-from object must become inactive
- `reset()` must not double-clean
- destruction after an early `reset()` must not retry cleanup
- even a throwing deleter should not cause a second attempt automatically

That last point is subtle and extremely important.

If the deleter throws and your object still thinks it is active, later destruction may call the deleter again. For many resources, that is worse than a leak. It can mean double-close, double-free, or logically invalid repeated cleanup.

So a robust minimal policy is:
- disarm first
- attempt cleanup second
- keep destructor `noexcept`

That is not the only possible policy, but it is a stable one for a foundational lab.

## Week 3 deep dive: manual container mechanics

### Why build a small vector-like container

The point is not to replace `std::vector`.

The point is to experience the implementation pressures that `std::vector` solves:
- storage vs object lifetime
- element construction vs raw bytes
- destruction order
- capacity bounds
- reallocation semantics
- exception guarantees

### The storage/object distinction

This is the moment when many C++ learners level up.

Raw storage is not the same thing as an object.

You can have memory that is large enough to hold `T` without a `T` existing there yet.

That is why vector-like code needs operations such as:
- placement construction / `std::construct_at`
- destruction / `std::destroy_at`
- careful tracking of how many live elements currently exist

This is the kind of thing higher-level languages hide from you entirely.

### Exception safety levels you should know now

#### Basic guarantee

If an operation fails:
- the object remains valid
- invariants hold
- some state change may already have happened

#### Strong guarantee

If an operation fails:
- the operation behaves as if it never happened
- previous observable state remains unchanged

#### No-throw guarantee

The operation itself promises not to throw.

For Phase A, the important thing is not memorizing formal definitions. It is learning to ask:

> “If construction of the new element throws halfway through, what exact state is this container in?”

That question is the beginning of real C++ implementation thinking.

### Iterator invalidation and reference invalidation

Container growth changes what previously borrowed references point to.

That means a user of the container must know:
- which operations preserve references/iterators
- which operations invalidate them
- whether the type documents those boundaries clearly

This becomes more important in later phases when views and pipelines appear.

## Common failure modes in Phase A

### 1. Confusing “non-owning” with “safe”

A view is often lightweight and expressive. It is not automatically safe.

### 2. Writing ownership comments instead of ownership types

If a type owns something, the type should encode that fact. Comments are not enough.

### 3. Forgetting that moved-from must still be valid

Moved-from objects do not need to be useful, but they do need to remain destructible and non-dangerous.

### 4. Treating exceptions as rare edge cases you can ignore

In C++, exceptions are also a design stress-test. Even if your project rarely throws in practice, thinking through failure paths reveals structural bugs.

### 5. Treating raw storage like a live object

If construction never happened, destruction must not happen. If destruction already happened, use must not continue.

## Testing expectations for this phase

Phase A tests should focus on behavior and invariants, not just happy-path demos.

You should expect tests like:
- correct size and access for borrowed view types
- out-of-range behavior if that is your chosen contract
- exactly-once cleanup under move and reset
- cleanup during exception unwinding
- overflow or growth edge cases in containers
- destruction of partially constructed state

When designing tests in this phase, keep asking:

> “What would go wrong if ownership or lifetime state were wrong by one step?”

That is often where the best tests come from.

## Performance and cost model for this phase

At this stage, performance is not about heroic optimization. It is about recognizing cost categories.

### Cheap abstractions in this phase

Usually cheap:
- copying a non-owning view
- moving an owning wrapper
- calling a small inline helper

Potentially expensive:
- deep copies of owning buffers
- repeated allocations
- repeated reallocation under growth
- unnecessary exception-heavy paths in hot code

The important skill is not “optimize everything.” It is:

> know which operations are conceptually cheap and which ones need measurement later.

## How this phase connects to later phases

Phase B depends on Phase A heavily.

You cannot design good generic APIs if you do not already understand:
- ownership boundaries
- moved-from state
- exception guarantees
- when references become invalid

Phase C depends on it too.

You cannot write concurrency or allocator code safely if your basic cleanup and lifetime rules are still fuzzy.

Phase D depends on all of it.

The final mini runtime is only coherent if its resource and object boundaries are coherent.

## Common interview-style explanations

### “What is RAII in practice?”

A good answer:

> RAII ties resource lifetime to object lifetime so cleanup happens automatically when scope ends. The main benefit is not convenience alone; it makes normal exits and exceptional exits obey the same cleanup rule.

### “What is the difference between an owning type and a view?”

A good answer:

> An owning type controls lifetime and cleanup of the underlying resource. A view is non-owning and only borrows access. That makes views cheaper to pass, but the caller must ensure the referenced storage outlives the view.

### “Why implement a small vector manually if `std::vector` already exists?”

A good answer:

> The purpose is not to replace `std::vector`; it is to understand storage lifetime, object lifetime, destruction, exception safety, and invalidation rules by being forced to make those decisions explicitly.

## Exit criteria for Phase A

You are ready to leave Phase A when you can:

- explain why a non-owning abstraction is useful and what it cannot guarantee
- implement a move-only RAII helper without double cleanup
- explain why destructors need a clear exception policy
- describe the difference between raw storage and constructed objects
- reason about what happens when a container operation throws halfway through

If any of those still feel fuzzy, spend more time here before trying to “compensate” with advanced features.

## Related repo artifacts

- [[../glossary/week-01-core-terms|Week 1 Core Terms]]
- [[../../notes/weekly/week-01|Week 1 Notes]]
- [[../../notes/weekly/week-02|Week 2 Notes]]
- [[../../missions/week-01-project-bootstrap-and-value-semantics/README|Week 1 Mission]]
- [[../../missions/week-02-raii-and-resource-management/README|Week 2 Mission]]
- [[../../missions/week-03-small-container-and-exception-safety/README|Week 3 Mission]]
- [[../superpowers/specs/2026-04-08-cpp-engineering-mastery-lab-design|Mastery Lab Design Spec]]
