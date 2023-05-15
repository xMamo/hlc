#include "sarray.h"

#include <stdint.h>

static void* _array_iterator_next(void* sarray) {
  return sarray_iterator_next(sarray);
}

static void _array_iterator_skip(void* iterator, size_t count) {
  sarray_iterator_skip(iterator, count);
}

const Iterator_methods array_iterator_iterator_methods = {
  .next = _array_iterator_next,
  .skip = _array_iterator_skip,
};

static Layout _array_iterator_layout(const void* sarray) {
  return LAYOUT(Sarray_iterator);
}

static const Iterator_methods* _array_iterator(const void* _sarray, void* _out_iterator) {
  const Sarray* sarray = _sarray;
  Sarray_iterator* out_iterator = _out_iterator;
  *out_iterator = sarray_iterator(sarray);
  return &array_iterator_iterator_methods;
}

const Iterable_methods sarray_iterable_methods = {
  .iterator_layout = _array_iterator_layout,
  .iterator = _array_iterator,
};

size_t sarray_count(const Sarray* sarray) {
  return sarray->count;
}

Layout sarray_item_layout(const Sarray* sarray) {
  return sarray->item_layout;
}

void* sarray_head(const Sarray* sarray) {
  return sarray->items;
}

void* sarray_at(const Sarray* sarray, size_t index) {
  if (index >= sarray->count)
    return NULL;

  // if (sarray->item_layout.size * index > SIZE_MAX)
  if (sarray->item_layout.size > SIZE_MAX / index)
    return NULL;

  return &((char*)sarray->items)[sarray->item_layout.size * index];
}

Sarray_iterator sarray_iterator(const Sarray* sarray) {
  return (Sarray_iterator){
    .sarray = sarray,
    .index = 0,
  };
}

void* sarray_iterator_next(Sarray_iterator* iterator) {
  void* item = sarray_at(iterator->sarray, iterator->index);

  if (item == NULL)
    return NULL;

  iterator->index += 1;
  return item;
}

void sarray_iterator_skip(Sarray_iterator* iterator, size_t count) {
  if (iterator->index >= iterator->sarray->count)
    return;

  // if (iterator->index + count > iterator->sarray->count)
  if (count > iterator->sarray->count - iterator->index) {
    count = iterator->sarray->count - iterator->index;
  }

  iterator->index += count;
}
