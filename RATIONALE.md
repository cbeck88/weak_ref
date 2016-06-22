## Rationale

A common need in C++ is to share pointers to objects with other objects who don't
necessarily own that object. If you are *sure* that those other objects won't
outlive the first object, then you can use a raw pointer or reference.

If you aren't sure or can't guarantee that, then within the standard library,
you can use a combination of `std::shared_ptr` and `std::weak_ptr`. If the first
object is owned by a `std::shared_ptr`, then the other objects can be given
`std::weak_ptr`'s to it. Then, when they want to use it, they lock it to check
if it is still there.

However, sometimes this is not the most appropriate or efficient solution.

- What if the first object isn't owned by a `std::shared_ptr`, or you just aren't
 willing to commit users of your code to that?

- `std::shared_ptr` has some significiant overhead. It uses atomics for thread-safety,
 which you might not need. It supports multiple ownership of objects. What if there
 will only ever be one owner of the object? In the `std::weak_ptr` model, as soon
 as the `weak_ptr` is locked, then it *obtains* ownership, becoming a shared owner
 and blocking another thread from deleting the object. But again, if I'm not concerned
 about that, then really my object only has one owner, and `weak_ptr` doesn't need to
 take ownership.

To make things very concrete: suppose I have some kind of object that performs some
kind of service. There are various ways of manipulating the object while it is alive.
Now, I want to provide a gui interface to the object, and in some existing gui framework,
this means that I need to create delegate objects which refer to the object and pass them
 to the gui. Now, perhaps the GUI uses its own internal resource management -- I can't know
exactly when it will destroy those delegates. But, I do what to delete my service when it's
no longer in use, and I don't really want to have a bunch of dangling pointers floating around
in these delegates held by the GUI. At the same time, I don't really want the GUI to be able to
"take ownership" of my service. As long as there are not multiple threads involved, this
should be feasible.

## Solution

The idea of `weak_ref` is quite simple. Suppose I need to share a reference to an object `t`
of type `T` with these delegates. First, we create an object of type `unique_ref` initialized
with `t`. This creates a reference-counted control structure on the stack, which manages a
pointer to `t`. From the `unique_ref`, `weak_ref`'s can be created, which increment the counter
when they are created, and decrement it when they are destroyed. When a `weak_ref` is locked,
it produces a raw pointer of type `t*`, by observing the value of the control structure.

The `unique_ref`, when it is destroyed, resets the `t*` in the control structure to `nullptr`.
This signals to all the `weak_ref`'s that the reference has expired and they can no longer
be locked successfully.

Clearly, the `weak_ref`'s are safe, so long as the `unique_ref` does not outlive `t`. This
can be easily ensured by storing the `unique_ref` with the owner of `t`.

The `unique_ref` model is also flexible in that it need not be tied directly to the lifetime of `t`.
It could be used to create "sessions" within which users of some service are permitted to use it,
even if the object `t` is not actually destroyed after the session ends.

