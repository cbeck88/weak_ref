# weak_ref

[![Build Status](https://travis-ci.org/cbeck88/weak_ref.svg?branch=master)](http://travis-ci.org/cbeck88/weak_ref)
[![Boost licensed](https://img.shields.io/badge/license-Boost-blue.svg)](./LICENSE)

A single-header header-only library for providing a **weak_reference** type for C++11.

In some applications, this can be more efficient than using `std::shared_ptr` and
`std::weak_ptr`, while giving a somewhat similar interface.

`weak_ref` does not take ownership of the objects that it holds, it merely checks
the validity of a shared pointer, by checking if the original `unique_ref` still exists.
It can be used with objects that are allocated on the stack or as members of other objects.

`weak_ref` is not thread-safe. In applications where you need to pass these across
threads, use `std::shared_ptr`.

## Example Usage

```
int x = 5;

weak_ref<int> r1;

assert(r1.lock() == nullptr);

{
  unique_ref<int> foo{x};

  r1 = foo;

  assert(r1.lock());
  assert(*r1.lock() == 5);
}

assert(r1.lock() == nullptr);
```

## Licensing and Distribution

`weak_ref` is a single header file available under the boost software license.
