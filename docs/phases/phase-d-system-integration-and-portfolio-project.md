---
title: Phase D System Integration and Portfolio Project
tags:
  - cpp
  - phase-guide
  - integration
  - executor
  - system-design
aliases:
  - Phase D Guide
  - System Integration and Portfolio Project
---

# Phase D: System Integration and Portfolio Project

## Why this phase exists

A mastery project should not end with a pile of isolated techniques.

If all you have at the end is:
- one custom container
- one callable wrapper
- one allocator experiment
- one queue
- one thread pool

then you have learned useful pieces, but you have not yet proved that you can **compose them into a coherent engineering system**.

Phase D exists to close that gap.

This is the phase where the earlier modules stop being separate exercises and start becoming one system with boundaries, dependencies, failure modes, and tradeoffs that must make sense together.

Phase D covers Weeks 10-12:
- [[../../missions/week-10-task-runtime-skeleton/README|Week 10 Mission]]
- [[../../missions/week-11-resource-aware-execution-pipeline/README|Week 11 Mission]]
- [[../../missions/week-12-productization-explanation-and-evidence/README|Week 12 Mission]]

It also connects to:
- [[../glossary/week-10-system-integration-terms|Week 10 System Integration Terms]]

## Phase goal

By the end of Phase D, you should be able to:

- assemble previously built abstractions into a small coherent runtime-oriented system
- define boundaries between tasks, execution, resources, and failure propagation clearly
- explain why the system is shaped the way it is
- produce final tests, benchmarks, notes, and architecture explanation that make the project portfolio-worthy

The transition here is from:

> “I implemented several advanced C++ components.”

to:

> “I can integrate advanced C++ components into a system and explain the design tradeoffs.”

## The mental model for this phase

The best summary sentence is:

> Integration is where local decisions prove whether they were globally coherent.

Earlier phases let you make good local decisions about ownership, interfaces, runtime cost, and concurrency. Phase D asks the harder question:

- do those local decisions still make sense when they interact?

That is the essence of system design.

## Week 10 deep dive: task runtime skeleton

### Why a task runtime is the right integrator

A task runtime is a strong capstone shape for this lab because it naturally requires:
- callable abstraction
- move-aware work submission
- queue behavior
- worker execution
- lifetime coordination
- API boundary clarity

It is small enough to finish, but rich enough to expose real design decisions.

### What a “task” means in this lab

A task is not just a function call.

A task is a unit of work with at least some implicit policy around:
- who owns the callable
- when execution happens
- who receives the result
- what happens if execution cannot happen
- what the scheduler/executor promises about submission and completion

This matters because the final system should make those policies legible.

### Executor as a boundary object

An executor is useful because it separates:
- *what work is*
- from *where and when work runs*

That boundary is one of the most reusable ideas in systems work.

A clean executor abstraction lets you reason about:
- submission interface
- task lifetime transfer
- result retrieval
- dependency on worker infrastructure

Without making every caller depend on the full internals of the thread pool.

### What minimal coherence looks like

At Week 10, you do not need a feature-rich runtime. You need a runtime skeleton whose boundaries make sense.

That means:
- the task abstraction is understandable
- the executor is not leaking implementation details unnecessarily
- task submission and execution ownership are clear
- failure modes are defined at least minimally

This phase is not about maximal features. It is about architectural cleanliness.

## Week 11 deep dive: resource-aware pipeline integration

### Why resource awareness matters

A runtime that only executes functions is not yet a system.

A system becomes more realistic when execution interacts with resources:
- cached state
- registries
- lazily-created values
- staged processing
- failure propagation across multiple steps

That is why Week 11 introduces resource registry/cache and pipeline composition.

### Resource registry as a lifetime boundary

A resource registry is not just a map. It is a policy object.

It answers questions like:
- who owns created resources?
- when are they created?
- when are they reused?
- when are they removed?
- how stable are returned references?
- what happens if creation fails?

This is exactly the kind of system-level lifetime problem that simpler exercises often avoid.

### Pipeline composition

A pipeline is a sequence of transformations or stages, but the engineering challenge is not only “run these in order.”

The real questions are:
- how is failure represented?
- does a failed stage stop the pipeline?
- are resources shared across stages or passed through them?
- are stages lazy, eager, synchronous, asynchronous, or some combination?

In this lab, the pipeline should stay intentionally small. But even the small version should make failure semantics obvious.

### Failure propagation as design clarity

A lot of weak system code hides failure semantics until late.

A stronger design states them up front.

Examples:
- stage throws → pipeline converts to error state
- resource creation fails → caller receives error, registry stays consistent
- submission after shutdown → explicit failure rather than ambiguous behavior

This is one of the most important habits of this phase:

> do not leave failure behavior implicit when multiple abstractions interact.

## Week 12 deep dive: productization and explainability

### Why “productization” matters in a learning lab

It is tempting to think the learning is done once the code exists.

But real engineering maturity includes:
- making the repository understandable to another person
- turning implementation decisions into clear written rationale
- showing build, test, benchmark, and design evidence
- presenting the project as a coherent artifact rather than a private experiment

This is why Week 12 matters.

It is the difference between:
- “I built something”
and
- “I can present, defend, and reuse what I built.”

### README as system narrative

A good final README should not be a dumping ground. It should answer:
- what is this project?
- what problem shape does it model?
- what are the main modules?
- how do the phases connect?
- how do you build, test, and benchmark it?
- what are the most interesting tradeoffs?

Think of the README as the shortest path from zero context to meaningful understanding.

### Interview-ready explanation notes

These notes matter because they force you to compress your understanding.

If you can explain:
- why your executor boundary exists
- why the callable wrapper has its chosen ownership model
- why your queue shutdown behaves the way it does
- why your registry owns resources internally
- why your pipeline uses its chosen error representation

then you actually understand the system.

If you cannot explain those in clean language, that is usually a sign that either the design or your understanding still needs work.

### Benchmark and test evidence as portfolio material

For this phase, “works” should mean:
- tests pass
- benchmarks run
- architecture is documented
- tradeoffs are written down
- the result is teachable to another engineer

That makes the final project feel credible.

## Integration principles for this phase

### 1. Preserve boundaries from earlier phases

Do not flatten everything into one file or one giant abstraction just because integration begins.

The system should show that you still respect:
- ownership boundaries
- interface boundaries
- runtime policy boundaries

### 2. Prefer explicit connections over magical ones

When two parts interact, make the interaction understandable.

Good integration is not “everything can talk to everything.” It is:
- tasks are submitted through this boundary
- resources are obtained through this policy object
- pipelines propagate failure this way
- shutdown is coordinated by this owner

### 3. Reuse should prove the earlier design was sound

A good capstone does not only reuse modules because the spec said so. It should make reuse feel natural.

If reuse feels forced, that is a signal that the earlier interfaces may have been too narrow, too vague, or too tied to one specific use case.

### 4. Explanations are part of correctness

In a learning/portfolio system, undocumented design is partially unfinished design.

That does not mean documenting every line. It means documenting:
- why this component exists
- what guarantee it provides
- what it intentionally does not provide
- where the cost/risk boundaries are

## Common failure modes in Phase D

### 1. Treating integration as “connect everything somehow”

That produces systems that technically work but have no clear architecture.

### 2. Smearing responsibilities across components

If executor, queue, registry, and pipeline all partially own the same policy decisions, the result becomes hard to explain and harder to maintain.

### 3. Leaving shutdown semantics vague

Integration often reveals shutdown problems that were hidden in isolated components.

### 4. Hiding failure behavior

If a pipeline, registry, or executor can fail, users should know how that failure appears.

### 5. Assuming code quality alone makes the project portfolio-worthy

A capstone becomes strong when code, tests, benchmarks, docs, and explanation all align.

## Testing expectations for this phase

Tests in Phase D should verify system-level contracts.

Good examples:
- executor schedules and returns results through the expected boundary
- registry creates once and reuses consistently
- pipeline applies stages in order
- failure in a stage becomes the documented pipeline error state
- shutdown or invalid-state behavior is explicit and reproducible

The guiding question for this phase is:

> “What promise does this integrated system make to its caller?”

Tests should pin down those promises.

## Performance and cost model for this phase

You should not expect the final system to be industrially optimized.

But you should expect to be able to talk clearly about:
- which parts allocate
- which parts erase types and therefore may add indirection
- which parts synchronize and therefore may contend
- where pipeline composition may add abstraction overhead
- what benchmark results actually suggest, rather than what you hoped they would say

This is enough to make the project credible and technically discussable.

## How the final project tells a coherent story

A strong final story for this lab looks something like this:

1. Phase A taught you to manage lifetime and ownership intentionally.
2. Phase B taught you to design generic interfaces and callable abstractions clearly.
3. Phase C taught you to reason about runtime cost, shutdown, and concurrency.
4. Phase D integrates those decisions into a mini task runtime with resource-aware execution and explicit failure behavior.

That story is important because it means the project is not a pile of features. It is a cumulative argument for your understanding.

## Common interview-style explanations

### “Why is a small executor/runtime a good capstone?”

A good answer:

> Because it naturally forces composition of ownership, callable abstraction, queueing, concurrency, failure handling, and system boundaries. It is small enough to finish but rich enough to expose real design tradeoffs.

### “What makes a good system boundary in this project?”

A good answer:

> A good boundary separates responsibility cleanly: task submission should not leak queue internals, resource acquisition should have its own policy object, and pipeline failure semantics should be explicit instead of implicit.

### “What makes this project more than a collection of exercises?”

A good answer:

> The later phases reuse earlier modules in a coherent final runtime. The abstractions are not isolated demos; they become parts of one system with documented tradeoffs, tests, benchmarks, and clear architecture.

## Exit criteria for Phase D

You are done with this phase when you can:

- explain the architecture of the final mini runtime cleanly
- show how earlier modules were reused intentionally
- describe task, resource, and failure boundaries precisely
- present tests and benchmarks as evidence rather than decoration
- hand the repo to another engineer and give them a short path to understanding

That is when the lab stops being only a learning scaffold and becomes a real portfolio artifact.

## Related repo artifacts

- [[../glossary/week-10-system-integration-terms|Week 10 System Integration Terms]]
- [[../../missions/week-10-task-runtime-skeleton/README|Week 10 Mission]]
- [[../../missions/week-11-resource-aware-execution-pipeline/README|Week 11 Mission]]
- [[../../missions/week-12-productization-explanation-and-evidence/README|Week 12 Mission]]
- [[../superpowers/specs/2026-04-08-cpp-engineering-mastery-lab-design|Mastery Lab Design Spec]]
- [[../superpowers/plans/2026-04-08-cpp-engineering-mastery-lab-implementation-plan|Implementation Plan]]
