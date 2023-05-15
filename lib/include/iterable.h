#ifndef ITERABLE_H
#define ITERABLE_H

#include "iterator.h"
#include "layout.h"

typedef struct Iterable_methods {
  Layout (*iterator_layout)(const void* data);
  const Iterator_methods* (*iterator)(const void* data, void* out_iterator);
} Iterable_methods;

typedef struct Iterable {
  void* data;
  const Iterable_methods* methods;
} Iterable;

static inline Layout iterable_iterator_layout(Iterable iterable) {
  return iterable.methods->iterator_layout(iterable.data);
}

static inline const Iterator_methods* iterable_iterator(Iterable iterable, void* out_iterator) {
  return iterable.methods->iterator(iterable.data, out_iterator);
}

#endif
