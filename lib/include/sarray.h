#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

#include "iterable.h"
#include "iterator.h"
#include "layout.h"

typedef struct Sarray {
  void* items;
  size_t count;
  Layout item_layout;
} Sarray;

typedef struct Sarray_iterator {
  const Sarray* sarray;
  size_t index;
} Sarray_iterator;

extern const Iterable_methods sarray_iterable_methods;

size_t sarray_count(const Sarray* sarray);

Layout sarray_item_layout(const Sarray* sarray);

void* sarray_head(const Sarray* sarray);

void* sarray_at(const Sarray* sarray, size_t index);

Sarray_iterator sarray_iterator(const Sarray* sarray);

void* sarray_iterator_next(Sarray_iterator* iterator);

void sarray_iterator_skip(Sarray_iterator* iterator, size_t count);

#endif
