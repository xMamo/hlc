#include "allocator.h"

#include <stddef.h>
#include <stdlib.h>

static void* _allocate(void* data, size_t size) {
  (void)data;
  return malloc(size);
}

static void* _reallocate(void* data, void* pointer, size_t size) {
  (void)data;
  return realloc(pointer, size);
}

static void _free(void* data, void* pointer) {
  (void)data;
  free(pointer);
}

static const Allocator_methods _allocator_methods = {
  .allocate = _allocate,
  .reallocate = _reallocate,
  .free = _free,
};

const Allocator heap_allocator = {
  .methods = NULL,
  .info = &_allocator_methods,
};
