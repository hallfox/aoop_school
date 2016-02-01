#ifndef _DEQUE_H_
#define _DEQUE_H_

// Purely for testing before I turn this into a macro
typedef int _type;
typedef _type *_type_ptr;

// Forward decls
struct Deque__type;
struct Deque__type_Iterator;

typedef struct Deque__type {
  // "Private" fields
  int _size;
  int _cap;

  _type_ptr _ring_head;
  _type_ptr _ring_tail;

  bool (*_cmp)(const _type &, const _type &);

  // "Public" fields
  const char *type_name;

  // Functions
  int (*size)(const Deque__type *deq);
  bool (*empty)(const Deque__type *deq);
  void (*push_front)(const Deque__type *deq, const _type& elem);
  void (*push_back)(const Deque__type *deq, const _type& elem);
  void (*pop_front)(const Deque__type *deq);
  void (*pop_back)(const Deque__type *deq);
  _type& (*front)(const Deque__type *deq);
  _type& (*back)(const Deque__type *deq);
  void (*dtor)(Deque__type *deq);
  void (*clear)(Deque__type *deq);
  _type& (*at)(Deque__type *deq, int i);
  Deque__type_Iterator (*begin)(Deque__type *deq);
  Deque__type_Iterator (*end)(Deque__type *deq);
} Deque__type;

typedef struct Deque__type_Iterator {
  void (*inc)(Deque__type_Iterator *it);
  _type& (*deref)(Deque__type_Iterator *it);
} Deque__type_Iterator;

// Implementations

int _size__type(const Deque__type *deq) {
  return deq->_size;
}

bool _empty__type(const Deque__type *deq) {
  return deq->_size == 0;
}

void _push_back__type(const Deque__type *deq, const _type& elem) {
  // TODO
}

void _push_front__type(const Deque__type *deq, const _type& elem) {
  // TODO
}

_type& _front__type(const Deque__type *deq) {
  // TODO
}

_type& _back__type(const Deque__type *deq) {
  // TODO
}

void _dtor__type(Deque__type *deq) {
  // TODO
}

void _clear__type(Deque__type *deq) {
  // TODO
}

_type& _at__type(Deque__type *deq, int i) {
  // TODO
}

Deque__type_Iterator _begin__type(Deque__type *deq) {
  // TODO
}

Deque__type_Iterator _end__type(Deque__type *deq) {
  // TODO
}

void _inc__type(Deque__type_Iterator *it) {
  // TODO
}

_type& _deref__type(Deque__type_Iterator *it) {
  // TODO
}

void _pop_front__type(const Deque__type *deq) {
  // TODO
}

void _pop_back__type(const Deque__type *deq) {
  // TODO
}

// Constructor

void Deque__type_ctor(Deque__type *deq, bool (*_cmp)(const _type &, const _type &)) {
  deq->_size = 0;
  deq->_cap = 0;
  deq->_cmp = _cmp;
  deq->_ring_head = nullptr;
  deq->_ring_tail = nullptr;
  // TODO Initialize array for storing the good stuff

  deq->type_name = "_type";

  deq->size = &_size__type;
  deq->empty = &_empty__type;
  deq->push_front = &_push_front__type;
  deq->push_back = &_push_back__type;
  deq->pop_front = &_pop_front__type;
  deq->pop_back = &_pop_back__type;
  deq->front = &_front__type;
  deq->back = &_back__type;
  deq->dtor = &_dtor__type;
  deq->clear = &_clear__type;
  deq->at = &_at__type;
  deq->begin = &_begin__type;
  deq->end = &_end__type;
}

// Comparison

bool Deque__type_equal(const Deque__type& deq1, const Deque__type& deq2) {
  // TODO
  return false;
}


#endif /* _DEQUE_H_ */
