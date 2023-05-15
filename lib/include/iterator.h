#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Iterator_methods {
  void* (*next)(void* data);
  void (*skip)(void* data, size_t count);
} Iterator_methods;

typedef struct Iterator {
  void* data;
  const Iterator_methods* methods;
} Iterator;

static inline void* iterator_next(Iterator iterator) {
  return iterator.methods->next(iterator.data);
}

static inline void iterator_skip(Iterator iterator, size_t count) {
  iterator.methods->skip(iterator.data, count);
}

#endif
