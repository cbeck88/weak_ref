#include "weak_ref.hpp"

#include <cassert>

int main() {
  using namespace nonstd;

  int x = 5;

  int y = 19;

  weak_ref<int> r1;
  weak_ref<int> r2;
  weak_ref<int> r3;

  assert(!r1.lock());
  assert(!r2.lock());
  assert(!r3.lock());

  {
    master_ref<int> foo{&x};

    r1 = foo;

    assert(r1.lock());
    assert(r1.lock());
    assert(*r1.lock() == 5);

    assert(!r2.lock());
    assert(!r3.lock());

    assert(foo.weak_ref_count() == 1);

    x = 6;
    assert(r1.lock());
    assert(*r1.lock() == 6);


    r2 = r1;

    assert(r1.lock());
    assert(*r1.lock() == 6);
    assert(r2.lock());
    assert(*r2.lock() == 6);

    master_ref<int> bar{&y};

    r3 = bar;

    assert(r1.lock());
    assert(*r1.lock() == 6);
    assert(r2.lock());
    assert(*r2.lock() == 6);
    assert(r3.lock());
    assert(*r3.lock() == 19);

    r2 = r3;

    assert(r1.lock());
    assert(*r1.lock() == 6);
    assert(r2.lock());
    assert(*r2.lock() == 19);
    assert(r3.lock());
    assert(*r3.lock() == 19);

    r2.reset();

    assert(r1.lock());
    assert(*r1.lock() == 6);
    assert(!r2.lock());
    assert(r3.lock());
    assert(*r3.lock() == 19);

    r2 = r1;

    assert(r1.lock());
    assert(*r1.lock() == 6);
    assert(r2.lock());
    assert(*r2.lock() == 6);
    assert(r3.lock());
    assert(*r3.lock() == 19);

    foo.reset();

    assert(!r1.lock());
    assert(!r2.lock());
    assert(r3.lock());
    assert(*r3.lock() == 19);
  }

  assert(!r1.lock());
  assert(!r2.lock());
  assert(!r3.lock());

  weak_ref<int> r4;

  assert(!r1.lock());
  assert(!r2.lock());
  assert(!r3.lock());
  assert(!r4.lock());

  {
    master_ref<int> foo{&x};
    master_ref<int> bar{&y};

    r1 = foo;
    r2 = foo;
    r3 = bar;
    r4 = bar;

    assert(r1.weak_ref_count() == 2);
    assert(r2.weak_ref_count() == 2);
    assert(r3.weak_ref_count() == 2);
    assert(r4.weak_ref_count() == 2);

    assert(*r1.lock() == 6);
    assert(*r2.lock() == 6);
    assert(*r3.lock() == 19);
    assert(*r4.lock() == 19);

    r3 = r2;

    assert(r1.weak_ref_count() == 3);
    assert(r2.weak_ref_count() == 3);
    assert(r3.weak_ref_count() == 3);
    assert(r4.weak_ref_count() == 1);

    assert(*r1.lock() == 6);
    assert(*r2.lock() == 6);
    assert(*r3.lock() == 6);
    assert(*r4.lock() == 19);

    foo = bar;
    assert(!r1.lock());
    assert(!r2.lock());
    assert(!r3.lock());
    assert(r4.lock());

    r1 = foo;
    r2 = foo;

    assert(r1.weak_ref_count() == 2);
    assert(r2.weak_ref_count() == 2);
    assert(r3.weak_ref_count() == 0);
    assert(r4.weak_ref_count() == 1);

    y = 27;

    assert(*r1.lock() == 27);
    assert(*r2.lock() == 27);
    assert(!r3.lock());
    assert(*r4.lock() == 27);

    bar.reset();

    assert(r1.weak_ref_count() == 2);
    assert(r2.weak_ref_count() == 2);
    assert(r3.weak_ref_count() == 0);
    assert(r4.weak_ref_count() == 0);

    bar = foo;
    r3 = bar;
    r4 = r3;

    assert(r1.weak_ref_count() == 2);
    assert(r2.weak_ref_count() == 2);
    assert(r3.weak_ref_count() == 2);
    assert(r4.weak_ref_count() == 2);
  }

  assert(!r1.lock());
  assert(!r2.lock());
  assert(!r3.lock());
  assert(!r4.lock());

  {
    weakly_referenced<int> z{-85};

    auto r5 = z.get_weak_ref();

    assert(r5.lock());
    assert(*r5.lock() == -85);

    r1 = z.get_weak_ref();

    assert(r1.weak_ref_count() == 2);
    assert(r5.weak_ref_count() == 2);
    assert(r1.lock() == r5.lock());
  }

  assert(!r1.lock());
  assert(!r2.lock());
  assert(!r3.lock());
  assert(!r4.lock());

  {
    weakly_referenced<int> q{19};
    auto r = q.get_weak_ref();
    auto s = q.get_weak_ref();
    auto t = q.get_weak_ref();

    assert(t.lock());

    r.reset();
    s.reset();
    t.reset();

    assert(!t.lock());
  }
}
