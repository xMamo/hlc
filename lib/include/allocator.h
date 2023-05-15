#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef struct Allocator_methods {
  void* (*allocate)(void* data, size_t size);
  void* (*reallocate)(void* data, void* pointer, size_t size);
  void (*free)(void* data, void* pointer);
} Allocator_methods;

typedef struct Allocator {
  void* methods;
  const Allocator_methods* info;
} Allocator;

extern const Allocator heap_allocator;

static inline void* allocator_allocate(Allocator allocator, size_t size) {
  return allocator.info->allocate(allocator.methods, size);
}

static inline void* allocator_reallocate(Allocator allocator, void* pointer, size_t size) {
  return allocator.info->reallocate(allocator.methods, pointer, size);
}

static inline void allocator_free(Allocator allocator, void* pointer) {
  allocator.info->free(allocator.methods, pointer);
}

#endif
