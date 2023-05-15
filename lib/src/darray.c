#include "darray.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

typedef struct _Darray {
  char* items;
  size_t count;
  size_t capacity;
  Layout item_layout;
  Allocator allocator;
} _Darray;

typedef struct _Darray_iterator {
  const _Darray* darray;
  size_t index;
} _Darray_iterator;

const Layout darray_layout = LAYOUT(_Darray);

static const Iterator_methods _darray_iterator_iterator_methods = {
  .next = darray_iterator_next,
  .skip = darray_iterator_skip,
};

static const Iterator_methods* _darray_iterator(const void* darray, void* out_iterator) {
  darray_iterator(darray, out_iterator);
  return &_darray_iterator_iterator_methods;
}

const Iterable_methods darray_iterable_methods = {
  .iterator_layout = darray_iterator_layout,
  .iterator = _darray_iterator,
};

void darray_init(void* darray, Layout item_layout) {
  darray_init_with(darray, item_layout, heap_allocator);
}

void darray_init_with(void* _darray, Layout item_layout, Allocator allocator) {
  _Darray* darray = _darray;

  darray->items = NULL;
  darray->count = 0;
  darray->capacity = 0;
  darray->item_layout = item_layout;
  darray->allocator = allocator;
}

void darray_destroy(void* _darray) {
  _Darray* darray = _darray;

  allocator_free(darray->allocator, darray->items);
  darray->items = NULL;
  darray->count = 0;
  darray->capacity = 0;
}

size_t darray_count(const void* _darray) {
  const _Darray* darray = _darray;
  return darray->count;
}

size_t darray_capacity(const void* _darray) {
  const _Darray* darray = _darray;
  return darray->capacity;
}

Layout darray_item_layout(const void* _darray) {
  const _Darray* darray = _darray;
  return darray->item_layout;
}

Allocator darray_allocator(const void* _darray) {
  const _Darray* darray = _darray;
  return darray->allocator;
}

bool darray_reserve(void* _darray, size_t count) {
  _Darray* darray = _darray;

  // if (darray->count + count > SIZE_MAX)
  if (darray->count > SIZE_MAX - count)
    return false;

  size_t new_count = darray->count + count;
  size_t new_capacity = darray->capacity;

  if (new_capacity < new_count && new_capacity == 0) {
    new_capacity = 1;
  }

  while (new_capacity < new_count) {
    // if (new_capacity * 2 > SIZE_MAX)
    if (new_capacity > SIZE_MAX / 2)
      return false;

    new_capacity = new_capacity * 2;
  }

  if (new_capacity > darray->capacity) {
    if (darray->item_layout.size != 0) {
      // if (darray->item_layout->size * new_capacity > SIZE_MAX)
      if (darray->item_layout.size > SIZE_MAX / new_capacity)
        return false;

      char* new_items = allocator_reallocate(
        darray->allocator,
        darray->items,
        darray->item_layout.size * new_capacity
      );

      if (new_items == NULL)
        return false;

      darray->items = new_items;
    }

    darray->capacity = new_capacity;
  }

  return true;
}

void darray_fit(void* _darray) {
  _Darray* darray = _darray;

  size_t new_capacity = darray->capacity;

  while (darray->count <= new_capacity / 2 && new_capacity != 0) {
    new_capacity /= 2;
  }

  if (new_capacity < darray->capacity) {
    if (darray->item_layout.size != 0) {
      if (new_capacity != 0) {
        char* new_items = allocator_reallocate(
          darray->allocator,
          darray->items,
          darray->item_layout.size * new_capacity
        );

        if (new_items != NULL) {
          darray->items = new_items;
          darray->capacity = new_capacity;
        }
      } else {  // if (new_capacity == 0)
        allocator_free(darray->allocator, darray->items);
        darray->items = NULL;
        darray->capacity = 0;
      }
    } else {  // if (darray->item_size == 0)
      darray->capacity = new_capacity;
    }
  }
}

bool darray_insert(void* darray, size_t index, const void* item) {
  return darray_insert_many(darray, index, item, 1);
}

bool darray_insert_many(void* _darray, size_t index, const void* items, size_t count) {
  _Darray* darray = _darray;

  if (index > darray->count || !darray_reserve(darray, count))
    return false;

  memmove(
    &darray->items[darray->item_layout.size * (index + count)],
    &darray->items[darray->item_layout.size * index],
    darray->item_layout.size * (darray->count - index)
  );

  memmove(
    &darray->items[darray->item_layout.size * index],
    items,
    darray->item_layout.size * count
  );

  darray->count += count;

  return true;
}

void darray_remove(void* darray, size_t index) {
  darray_remove_many(darray, index, 1);
}

void darray_remove_many(void* _darray, size_t index, size_t count) {
  _Darray* darray = _darray;

  if (index >= darray->count)
    return;

  // if (index + count > darray->count)
  if (count > darray->count - index) {
    count = darray->count - index;
  }

  size_t new_count = darray->count - count;

  memmove(
    &darray->items[darray->item_layout.size * index],
    &darray->items[darray->item_layout.size * (index + count)],
    darray->item_layout.size * (darray->count - (index + count))
  );

  darray->count = new_count;
  darray_fit(darray);
}

bool darray_push(void* darray, const void* item) {
  return darray_insert(darray, darray_count(darray), item);
}

bool darray_pop(void* darray, void* out_item) {
  size_t length = darray_count(darray);

  if (length == 0)
    return false;

  if (out_item != NULL) {
    memmove(out_item, darray_at(darray, length - 1), darray_item_layout(darray).size);
  }

  darray_remove(darray, length - 1);
  return true;
}

void* darray_head(const void* darray) {
  return darray_at(darray, 0);
}

void* darray_at(const void* _darray, size_t index) {
  const _Darray* darray = _darray;
  return index < darray->count ? &darray->items[darray->item_layout.size * index] : NULL;
}

Layout darray_iterator_layout(const void* darray) {
  return LAYOUT(_Darray_iterator);
}

void darray_iterator(const void* _darray, void* _out_iterator) {
  const _Darray* darray = _darray;
  _Darray_iterator* out_iterator = _out_iterator;

  out_iterator->darray = darray;
  out_iterator->index = 0;
}

void* darray_iterator_next(void* _iterator) {
  _Darray_iterator* iterator = _iterator;

  void* item = darray_at(iterator->darray, iterator->index);

  if (item == NULL)
    return NULL;

  iterator->index += 1;
  return item;
}

void darray_iterator_skip(void* _iterator, size_t count) {
  _Darray_iterator* iterator = _iterator;

  if (iterator->index >= iterator->darray->count)
    return;

  // if (iterator->index + count > iterator->darray->count)
  if (count > iterator->darray->count - iterator->index) {
    count = iterator->darray->count - iterator->index;
  }

  iterator->index += count;
}
