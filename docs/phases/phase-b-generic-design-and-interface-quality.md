---
title: Phase B Generic Design and Interface Quality
tags:
  - cpp
  - phase-guide
  - templates
  - concepts
  - type-erasure
aliases:
  - Phase B Guide
  - Generic Design and Interface Quality
---

# Phase B: Generic Design and Interface Quality

## Why this phase exists

After Phase A, you know enough about ownership and lifetime to stop writing dangerous code by accident. But that still does not mean your interfaces are good.

Many C++ learners get stuck in an awkward middle state:
- they know templates exist, but use them mechanically
- they know about forwarding, but not when it is appropriate
- they can make an API compile, but not make it understandable
- they can wrap a callable, but not reason about ownership or cost
- they can make an iterator type, but not explain what category it models or what guarantees it gives

Phase B exists to fix that.

This is the phase where you start treating C++ interfaces as a form of **communication** instead of just a way to make generic code compile.

Phase B covers Weeks 4-6:
- [[../../missions/week-04-templates-constraints-and-generic-apis/README|Week 4 Mission]]
- [[../../missions/week-05-callable-design-and-type-erasure/README|Week 5 Mission]]
- [[../../missions/week-06-iterators-views-and-range-oriented-design/README|Week 6 Mission]]

It also connects directly to:
- [[../glossary/week-04-generic-design-terms|Week 4 Generic Design Terms]]

## Phase goal

By the end of Phase B, you should be able to:

- choose when a generic API is actually worth writing
- constrain template interfaces so misuse fails clearly
- reason about forwarding and overload behavior without guessing
- explain the ownership and cost model of callable wrappers
- design iterators and views that are small, clear, and intentionally limited

The real transition here is from:

> “I can write template code.”

to:

> “I can design a generic interface with understandable semantics and acceptable cost.”

## The mental model for this phase

The simplest useful sentence for Phase B is:

> Generic code is an API design problem first, and a syntax problem second.

That means when you build a generic abstraction, you must think about:
- what kinds of types are actually supported
- how misuse should fail
- what ownership is implied
- whether the abstraction adds hidden cost
- whether the abstraction hides too much or too little

## Week 4 deep dive: templates, constraints, and generic APIs

### Why template code becomes hard to read

Template-heavy code becomes unreadable when it answers the wrong question.

Bad generic code often asks:
- “How can I make this work for everything?”

Good generic code asks:
- “What exact class of types should this work for?”
- “How should that requirement be expressed?”
- “What would a user misunderstand here?”

That is why concepts and constraints matter.

### Template parameter design

A template parameter is not just a placeholder. It is a statement about the space of valid inputs.

Bad design:
- accepting more than you really support
- relying on template instantiation errors as your only validation
- making users debug internals when they violate a simple precondition

Better design:
- constrain to the shape you actually need
- name semantic requirements clearly
- keep generic surface area as small as possible

### Concepts and `requires`

The point of concepts is not only correctness. It is communication.

A constrained interface says:
- this API is meant for these kinds of types
- other kinds are out of scope
- the failure boundary is deliberate

That makes code easier to read and easier to misuse less catastrophically.

### Forwarding references and overload control

This area causes a lot of confusion because the syntax is compact and the behavior is subtle.

The useful practical model is:

- not every API needs perfect forwarding
- forwarding is valuable when you are passing construction arguments through to another object
- forwarding is dangerous when used mechanically, because it makes overload sets and value-category behavior harder to reason about

When deciding whether to forward, ask:

1. Is this API fundamentally a pass-through construction or invocation layer?
2. Does preserving value category meaningfully help callers?
3. Does the added complexity still keep the API readable?

If the answer to the third is no, simplify.

### Compile-time diagnostics are part of interface quality

A type error is not only a type-system event. It is also user experience.

Bad generic libraries make users read pages of substitution failure from deep internals.

Better ones fail near the interface boundary and make the conceptual contract visible.

That is why in this phase you should think about diagnostics as a product requirement, not an afterthought.

## Week 5 deep dive: callable wrappers and type erasure

### Why callable wrappers matter

Callables are everywhere in modern C++:
- callbacks
- deferred work
- task systems
- algorithms
- visitor-like interfaces
- event pipelines

But “callable” is not one thing. Different wrappers encode different policies.

### Three big callable questions

Whenever you design a callable wrapper, ask:

1. **Does it own the callable?**
2. **Can it store move-only state?**
3. **What is the storage/cost model?**

Those three questions matter more than memorizing library names.

### `function_ref`-style thinking

A non-owning callable reference is like a function-oriented view.

It is useful when:
- the callable is guaranteed to outlive the wrapper
- you want no ownership and minimal overhead
- you are passing behavior temporarily through APIs

It is dangerous when:
- the wrapper escapes the lifetime of the referenced callable
- users assume it behaves like an owning wrapper

This mirrors the owning vs non-owning distinction from Phase A.

### `small_function`-style thinking

An owning callable wrapper with inline storage introduces a different tradeoff:
- more convenient lifetime model
- more general than a non-owning reference
- potentially more expensive
- requires clearer storage and movement policy

This is where type erasure becomes practical rather than magical.

### What type erasure really is

Type erasure is not “template avoidance.”

It is a technique for:
- storing different concrete types behind one stable runtime interface
- hiding implementation type from consumers
- shifting part of generic behavior from compile time to runtime

That shift is often worth it, but it is never free.

### Small object optimization (SOO)

Small object optimization is one of the most common performance-design tradeoffs in callable wrappers.

The idea:
- small callables live inside an inline buffer
- larger callables fall back to heap allocation or are disallowed

Why it matters:
- avoids heap allocation for common small lambdas
- improves locality
- complicates implementation and invariants

This is a good example of a recurring Phase B theme:

> An interface that looks tiny may hide a nontrivial cost model.

### Ownership semantics in callable design

A high-quality callable abstraction should make users infer the right lifetime story.

That means your API should make it clear whether:
- the callable is borrowed
- the callable is owned
- the callable can be empty
- moves invalidate prior wrappers
- invoking an empty wrapper is allowed or an error

If the wrapper’s semantics are not obvious, the interface is incomplete.

## Week 6 deep dive: iterators, views, and range-oriented design

### Why iterators still matter in the ranges era

It is tempting to think modern ranges make iterators less important. In reality, they make iterator design even more visible.

Ranges are not magic. They are structured ways to compose:
- iteration boundaries
- element access
- laziness
- adaptors
- ownership or borrowing relationships

To understand range-oriented APIs, you still need iterator fundamentals.

### Iterator category as a semantic contract

An iterator category is not merely a tag. It tells consumers what operations and complexity assumptions are valid.

For example, random access implies a different set of expectations than input iteration.

If you misclassify an iterator, generic algorithms may compile but make invalid assumptions about capability or cost.

That is why iterator categories belong in the same mental bucket as exception guarantees and ownership boundaries: they are semantic promises.

### Lazy vs eager evaluation

A view-like abstraction often delays work until iteration happens.

That can be excellent because:
- it avoids intermediate storage
- it composes well
- it makes pipelines elegant

But it also creates questions:
- how long must the underlying data live?
- when do predicates or transforms run?
- how many times are they evaluated?
- is the abstraction re-iterable?

These questions matter because laziness shifts when work happens, not whether it happens.

### “STL-like” does not mean “copy STL blindly”

A common trap is trying to imitate the standard library too early.

The standard library is a mature, highly generalized design space. Your learning implementations should borrow the right lessons:
- clear boundaries
- explicit semantics
- useful naming
- predictable behavior

But they should stay intentionally smaller.

Good educational design says:
- match the spirit of the interface where helpful
- do not reproduce industrial complexity before you understand the smaller core

## Common failure modes in Phase B

### 1. Making APIs generic when they should be specific

Not every function improves when templated.

Sometimes the best interface is concrete because that keeps semantics obvious.

### 2. Using forwarding references by reflex

Perfect forwarding is powerful, but it is often cargo-culted into places where a simpler overload or by-value API would be clearer.

### 3. Hiding cost behind nice syntax

Type erasure, lazy views, and erased callables can all look lightweight while hiding allocations, indirect calls, or lifetime constraints.

### 4. Treating constraints as decoration

Concepts are not ornamental. They define what your interface actually means.

### 5. Over-designing educational abstractions

Your wrapper or iterator does not need to compete with the standard library. It needs to teach the core tradeoff clearly.

## Testing expectations for this phase

Tests in Phase B should verify semantics, not only “works once in a demo.”

Good tests here include:
- valid vs invalid use cases for constrained APIs
- move-only callable support where promised
- empty or invalid wrapper behavior where applicable
- iterator traversal order and filtering behavior
- repeated iteration behavior if re-iteration is supposed to work
- behavior under borrowed underlying storage assumptions

A strong test question for this phase is:

> “If another engineer used this API without reading the implementation, what wrong assumption would they most likely make?”

That likely misunderstanding should have either a clearer interface or a test that fixes the contract in place.

## Performance and cost model for this phase

This phase is where you should start speaking more precisely about cost.

### Typical cost sources

- template instantiation complexity at compile time
- indirect invocation through type erasure
- heap allocation in owning callable wrappers
- predicate or transform overhead in lazy pipelines
- accidental copies when forwarding/value-category design is wrong

### Zero-cost abstraction discipline

“Zero-cost abstraction” does not mean “every abstraction is free.” It means:
- if an abstraction costs something, you should understand why
- if the abstraction is worth it, the cost is justified by design value
- if the abstraction is not worth it, simplify

This phase should train you to ask not just “does it compile?” but also:

- what gets instantiated?
- what gets stored?
- what gets copied?
- what gets invoked indirectly?
- what gets evaluated later?

## How this phase connects to later phases

Phase C depends on Phase B because runtime and concurrency code need robust interfaces.

You cannot build a sane thread pool or executor if you do not understand:
- how callable ownership works
- how erased callable storage behaves
- how borrowed vs owned pipelines differ
- how much abstraction cost you are taking on

Phase D depends on it because the final runtime is mostly a composition exercise. Composition quality is an interface-quality problem.

## Common interview-style explanations

### “Why use concepts instead of unconstrained templates?”

A good answer:

> Concepts make the intended contract explicit. They improve correctness, but just as importantly they improve diagnostics and make the API easier to understand at the call site.

### “What is type erasure and when is it useful?”

A good answer:

> Type erasure hides different concrete callable or object types behind one stable runtime interface. It is useful when you want flexibility at the API boundary, but you accept some runtime cost and implementation complexity to get it.

### “What is the difference between a borrowed callable wrapper and an owning one?”

A good answer:

> A borrowed wrapper like `function_ref` assumes the underlying callable outlives the wrapper, so it is cheap but has a stricter lifetime requirement. An owning wrapper stores the callable itself, which is more flexible but usually has a more complex storage and cost model.

## Exit criteria for Phase B

You are ready to leave Phase B when you can:

- justify why an interface should or should not be generic
- explain what your constraints mean semantically, not only syntactically
- describe the ownership policy of a callable wrapper clearly
- explain the cost model of a callable or range abstraction
- define what your iterator/view guarantees and what it does not

If those are still fuzzy, Phase C will feel harder because runtime code magnifies interface mistakes.

## Related repo artifacts

- [[../glossary/week-04-generic-design-terms|Week 4 Generic Design Terms]]
- [[../../missions/week-04-templates-constraints-and-generic-apis/README|Week 4 Mission]]
- [[../../missions/week-05-callable-design-and-type-erasure/README|Week 5 Mission]]
- [[../../missions/week-06-iterators-views-and-range-oriented-design/README|Week 6 Mission]]
- [[../superpowers/specs/2026-04-08-cpp-engineering-mastery-lab-design|Mastery Lab Design Spec]]
