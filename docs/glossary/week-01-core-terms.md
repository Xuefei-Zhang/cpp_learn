# Week 1 Core Terms

## Translation unit

A translation unit is the source file being compiled plus every header it includes after preprocessing. This matters because declarations in headers can end up compiled into many translation units.

## Linkage

Linkage describes whether the same name can refer to the same entity across translation units. External linkage allows sharing across files, internal linkage keeps a name local to one translation unit, and no linkage means the name is only meaningful in its own scope.

## Owning vs non-owning

An owning type controls a resource lifetime and is responsible for cleanup. A non-owning type only observes or refers to someone else’s storage, so the caller must ensure the referenced object outlives the view.

## Trivial type

A trivial type can be created, copied, and destroyed without custom behavior from user-defined special member functions. Triviality matters because it affects low-level operations, optimization opportunities, and what kinds of storage tricks are safe.

## Move

A move transfers usable state from one object into another without promising a deep copy. It is mainly about preserving correctness while allowing cheaper transfer of ownership or buffers.

## RAII

RAII means resource acquisition is initialization: resource ownership is tied to object lifetime so cleanup happens automatically in destructors. It is the core C++ tool for making success and failure paths obey the same cleanup rules.
