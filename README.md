# weak_ref

[![Build Status](https://travis-ci.org/cbeck88/weak_ref.svg?branch=master)](http://travis-ci.org/cbeck88/weak_ref)
[![Boost licensed](https://img.shields.io/badge/license-Boost-blue.svg)](./LICENSE)

A single-header header-only library for providing a **weak reference** type for C++11.

**weak ref** provides a non-intrusive way to share pointers to an object safely.
**weak ref** does not require that the target object is heap allocated, it can be
on the stack or the heap and managed in any way. This is *not* a smart pointer or
ownership management system.

Instead, the idea is that a `unique_ref` is bound to the target object, then
`weak_ref`'s are created from the `unique_ref`. When the `unique_ref` is
destroyed, the `weak_ref`'s can no longer be locked and return `nullptr`. So
long as the `unique_ref` does not outlive the target object, the pointers are
successfully shared and no dangling references occur.

When the goal is only to track the lifetime of a given object in a light-weight
manner, this can be more efficient than using `std::shared_ptr` and
`std::weak_ptr`, while giving a somewhat similar interface.

`weak_ref` is not thread-safe. In applications where you need to pass these across
threads, use `std::shared_ptr`.

## Example Usage

```
int x = 5;

weak_ref<int> r1;

assert(r1.lock() == nullptr);

{
  unique_ref<int> foo{&x};

  r1 = foo;

  assert(r1.lock());
  assert(*r1.lock() == 5);
}

assert(r1.lock() == nullptr);
```

## Licensing and Distribution

`weak_ref` is a single header file available under the boost software license.
