#ifndef _DEQUE_H_
#define _DEQUE_H_

#include <stdlib.h>

#define Deque_DEFINE(_type)                                             \
                                                                        \
/* Purely for testing before I turn this into a macro */                \
typedef _type *_type##_ptr;                                             \
                                                                        \
/* Forward decls */                                                     \
struct Deque_##_type;                                                   \
struct Deque_##_type##_Iterator;                                        \
                                                                        \
typedef struct Deque_##_type {                                          \
  /* "Private" fields */                                                \
  unsigned int _size;                                                   \
  unsigned int _cap;                                                    \
                                                                        \
  unsigned int _ring_head;                                              \
  unsigned int _ring_tail;                                              \
  _type##_ptr _ring;                                                    \
                                                                        \
  bool (*_cmp)(const _type &, const _type &);                           \
                                                                        \
  /* "Public" fields */                                                 \
  const char *type_name;                                                \
                                                                        \
  /* Functions */                                                       \
  int (*size)(const Deque_##_type *deq);                                \
  bool (*empty)(const Deque_##_type *deq);                              \
  void (*push_front)(Deque_##_type *deq, _type elem);                   \
  void (*push_back)(Deque_##_type *deq, const _type elem);              \
  void (*pop_front)(Deque_##_type *deq);                                \
  void (*pop_back)(Deque_##_type *deq);                                 \
  _type& (*front)(const Deque_##_type *deq);                            \
  _type& (*back)(const Deque_##_type *deq);                             \
  void (*dtor)(Deque_##_type *deq);                                     \
  void (*clear)(Deque_##_type *deq);                                    \
  _type& (*at)(Deque_##_type *deq, unsigned int i);                     \
  void (*expand)(Deque_##_type *deq);                                   \
  Deque_##_type##_Iterator (*begin)(Deque_##_type *deq);                \
  Deque_##_type##_Iterator (*end)(Deque_##_type *deq);                  \
} Deque_##_type;                                                        \
                                                                        \
typedef struct Deque_##_type##_Iterator {                               \
  /* Accessor fields */                                                 \
  Deque_##_type *_deq;                                                  \
  unsigned int _idx;                                                    \
                                                                        \
  void (*inc)(Deque_##_type##_Iterator *it);                            \
  void (*dec)(Deque_##_type##_Iterator *it);                            \
  _type& (*deref)(Deque_##_type##_Iterator *it);                        \
} Deque_##_type##_Iterator;                                             \
                                                                        \
/* Implementations */                                                   \
                                                                        \
int _size_##_type(const Deque_##_type *deq) {                           \
  return deq->_size;                                                    \
}                                                                       \
                                                                        \
bool _empty_##_type(const Deque_##_type *deq) {                         \
  return deq->_size == 0;                                               \
}                                                                       \
                                                                        \
void _push_front_##_type(Deque_##_type *deq, _type elem) {              \
  /* Make life easier by expanding just before the deque fills up */    \
  if (deq->_size + 1 >= deq->_cap) {                                    \
    deq->expand(deq);                                                   \
  }                                                                     \
                                                                        \
  unsigned int new_front;                                               \
  if (deq->empty(deq)) {                                                \
    new_front = deq->_ring_head;                                        \
  }                                                                     \
  else {                                                                \
    new_front = (deq->_ring_head + deq->_cap - 1) % deq->_cap;          \
  }                                                                     \
  /* Assuming copy */                                                   \
  deq->_ring[new_front] = elem;                                         \
  deq->_ring_head = new_front;                                          \
  deq->_size++;                                                         \
}                                                                       \
                                                                        \
/* NOTE pass by val */                                                  \
void _push_back_##_type(Deque_##_type *deq, const _type elem) {         \
  if (deq->_size + 1 >= deq->_cap) {                                    \
    deq->expand(deq);                                                   \
  }                                                                     \
                                                                        \
  unsigned int new_back;                                                \
  if (deq->empty(deq)) {                                                \
    new_back = deq->_ring_tail;                                         \
  }                                                                     \
  else {                                                                \
    new_back = (deq->_ring_tail + 1) % deq->_cap;                       \
  }                                                                     \
  /* Assuming copy */                                                   \
  deq->_ring[new_back] = elem;                                          \
  deq->_ring_tail = new_back;                                           \
  deq->_size++;                                                         \
}                                                                       \
                                                                        \
void _pop_front_##_type(Deque_##_type *deq) {                           \
  if (deq->empty(deq)) {                                                \
    /* FIXME Can't pop empty deque */                                   \
    return;                                                             \
  }                                                                     \
                                                                        \
  deq->_size--;                                                         \
  if (deq->_size > 0){                                                  \
    deq->_ring_head = (deq->_ring_head + 1) % deq->_cap;                \
  }                                                                     \
}                                                                       \
                                                                        \
void _pop_back_##_type(Deque_##_type *deq) {                            \
  if (deq->empty(deq)) {                                                \
    /* FIXME Can't pop empty deque */                                   \
    return;                                                             \
  }                                                                     \
                                                                        \
  deq->_size--;                                                         \
  if (deq->_size > 0){                                                  \
    deq->_ring_tail = (deq->_ring_tail + deq->_cap - 1) % deq->_cap;    \
  }                                                                     \
}                                                                       \
                                                                        \
_type& _front_##_type(const Deque_##_type *deq) {                       \
  /* FIXME What if deq is empty? */                                     \
  return deq->_ring[deq->_ring_head];                                   \
}                                                                       \
                                                                        \
_type& _back_##_type(const Deque_##_type *deq) {                        \
  /* FIXME What if deq is empty? */                                     \
  return deq->_ring[deq->_ring_tail];                                   \
}                                                                       \
                                                                        \
void _dtor_##_type(Deque_##_type *deq) {                                \
  free(deq->_ring);                                                     \
  deq->_ring = nullptr;                                                 \
}                                                                       \
                                                                        \
void _clear_##_type(Deque_##_type *deq) {                               \
  deq->_ring_head = deq->_cap / 2;                                      \
  deq->_ring_tail = deq->_ring_head;                                    \
  deq->_size = 0;                                                       \
}                                                                       \
                                                                        \
_type& _at_##_type(Deque_##_type *deq, unsigned int i) {                \
  if (i >= deq->_size) {                                                \
    /* FIXME out of bounds */                                           \
    /* Um so I guess the at() fn in the STL throws an exception */      \
    /* so I suppose this should too BUT operator[]() doesn't, */        \
    /* so I'm going to pretend this is actually operator[]() */         \
  }                                                                     \
  return deq->_ring[(deq->_ring_head + i) % deq->_cap];                 \
}                                                                       \
                                                                        \
void _expand_##_type(Deque_##_type *deq) {                              \
  /* Double the capacity and reserve space */                           \
  unsigned int old_cap = deq->_cap;                                     \
  deq->_cap *= 2;                                                       \
  realloc(deq->_ring, deq->_cap);                                       \
                                                                        \
  /* If the head is in front of the tail we need */                     \
  /* to unwrap the ring */                                              \
  if (deq->_ring_head > deq->_ring_tail) {                              \
    for (unsigned int i = 0; i <= deq->_ring_tail; i++) {               \
      deq->_ring[old_cap + i] = deq->_ring[i];                          \
    }                                                                   \
    /* Think about this some more */                                    \
    deq->_ring_tail += old_cap;                                         \
  }                                                                     \
}                                                                       \
                                                                        \
void _inc_##_type(Deque_##_type##_Iterator *it) {                       \
  it->_idx = (it->_idx + 1) % it->_deq->_cap;                           \
}                                                                       \
                                                                        \
void _dec_##_type(Deque_##_type##_Iterator *it) {                       \
  it->_idx = (it->_idx + it->_deq->_cap - 1) % it->_deq->_cap;          \
}                                                                       \
                                                                        \
_type& _deref_##_type(Deque_##_type##_Iterator *it) {                   \
  return it->_deq->_ring[it->_idx];                                     \
}                                                                       \
                                                                        \
Deque_##_type##_Iterator _begin_##_type(Deque_##_type *deq) {           \
  Deque_##_type##_Iterator it;                                          \
                                                                        \
  /* Woo shared ownership of pointers */                                \
  it._deq = deq;                                                        \
  it._idx = 0;                                                          \
  it.inc = &_inc_##_type;                                               \
  it.dec = &_dec_##_type;                                               \
  it.deref = &_deref_##_type;                                           \
                                                                        \
  return it;                                                            \
}                                                                       \
                                                                        \
Deque_##_type##_Iterator _end_##_type(Deque_##_type *deq) {             \
  Deque_##_type##_Iterator it;                                          \
                                                                        \
  /* Woo shared ownership of pointers */                                \
  it._deq = deq;                                                        \
  /* This couldn't possibly go wrong */                                 \
  if (deq->_size == deq->_cap) {                                        \
    it._idx = deq->_ring_head + deq->_size;                             \
  }                                                                     \
  else {                                                                \
    it._idx = (deq->_ring_head + deq->_size) % deq->_cap;               \
  }                                                                     \
  it.inc = &_inc_##_type;                                               \
  it.deref = &_deref_##_type;                                           \
                                                                        \
  /* As far as I'm concerned deref'ing the end iterator */              \
  /* in C++ leads to undef'd behavior, so I'm going to */               \
  /* give you the gun, just please don't pull the trigger */            \
                                                                        \
  return it;                                                            \
}                                                                       \
                                                                        \
/* Constructor */                                                       \
                                                                        \
void Deque_##_type##_ctor(Deque_##_type *deq, bool (*_cmp)(const _type &, const _type &)) { \
  deq->_size = 0;                                                       \
  deq->_cap = 11; /* default cap */                                     \
  deq->_cmp = _cmp;                                                     \
  deq->_ring_head = deq->_cap / 2;                                      \
  deq->_ring_tail = deq->_cap / 2;                                      \
  deq->_ring = (_type##_ptr) malloc(deq->_cap * sizeof(_type));         \
                                                                        \
  deq->type_name = #_type;                                              \
                                                                        \
  deq->size = &_size_##_type;                                           \
  deq->empty = &_empty_##_type;                                         \
  deq->push_front = &_push_front_##_type;                               \
  deq->push_back = &_push_back_##_type;                                 \
  deq->pop_front = &_pop_front_##_type;                                 \
  deq->pop_back = &_pop_back_##_type;                                   \
  deq->front = &_front_##_type;                                         \
  deq->back = &_back_##_type;                                           \
  deq->dtor = &_dtor_##_type;                                           \
  deq->clear = &_clear_##_type;                                         \
  deq->expand = &_expand_##_type;                                       \
  deq->at = &_at_##_type;                                               \
  deq->begin = &_begin_##_type;                                         \
  deq->end = &_end_##_type;                                             \
}                                                                       \
                                                                        \
/* Comparison */                                                        \
                                                                        \
bool Deque_##_type##_Iterator_equal(const Deque_##_type##_Iterator& it1, const Deque_##_type##_Iterator& it2) { \
  return it1._deq == it2._deq && it1._idx == it2._idx;                  \
}                                                                       \
                                                                        \
/* If we had const iterators these refs would be const but we don't so oh well */ \
bool Deque_##_type##_equal(Deque_##_type& deq1, Deque_##_type& deq2) {  \
  /* some easy checks */                                                \
  if (&deq1 == &deq2) {                                                 \
    return true;                                                        \
  }                                                                     \
  if (deq1.size(&deq1) != deq2.size(&deq2)) {                           \
    return false;                                                       \
  }                                                                     \
                                                                        \
  Deque_##_type##_Iterator it1 = deq1.begin(&deq1);                     \
  Deque_##_type##_Iterator it2 = deq2.begin(&deq2);                     \
  while (!Deque_##_type##_Iterator_equal(it1, deq1.end(&deq1)) &&       \
         !Deque_##_type##_Iterator_equal(it2, deq2.end(&deq2))) {       \
    if (deq1._cmp(it1.deref(&it1), it2.deref(&it2)) ||                  \
        deq2._cmp(it2.deref(&it2), it1.deref(&it1))) {                  \
      return false;                                                     \
    }                                                                   \
    it1.inc(&it1);                                                      \
    it2.inc(&it2);                                                      \
  }                                                                     \
                                                                        \
  return true;                                                          \
}


#endif /* _DEQUE_H_ */
