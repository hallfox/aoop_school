#include "Deque.hpp"
#include <stdio.h>
#include <assert.h>

bool
int_less(const int &o1, const int &o2) {
  return o1 < o2;
}

int main() {
  // Deque__type deq1, deq2;
  // Deque__type_ctor(&deq1, int_less);
  // Deque__type_ctor(&deq2, int_less);

  // deq1.push_back(&deq1, 1);
  // deq1.push_back(&deq1, 2);
  // deq1.push_back(&deq1, 3);
  // deq2.push_back(&deq2, 1);
  // deq2.push_back(&deq2, 2);
  // deq2.push_back(&deq2, 3);

  // assert(Deque__type_equal(deq1, deq2));

  // deq1.pop_back(&deq1);
  // assert(!Deque__type_equal(deq1, deq2));
  // deq1.push_back(&deq1, 4);
  // assert(!Deque__type_equal(deq1, deq2));

  // deq1.dtor(&deq1);
  // deq2.dtor(&deq2);

  return 0;
}
