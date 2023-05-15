#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Iterator_methods {
  void* (*next)(void* data);
  void (*skip)(void* data, size_t count);
} Iterator_methods;

typedef struct Iterator {
  void* methods;
  const Iterator_methods* info;
} Iterator;

static inline void* iterator_next(Iterator iterator) {
  return iterator.info->next(iterator.methods);
}

static inline void iterator_skip(Iterator iterator, size_t count) {
  iterator.info->skip(iterator.methods, count);
}

#endif
