#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "allocator.h"
#include "comparator.h"
#include "layout.h"

extern const Layout map_layout;

void map_init(
  void* map,
  Layout key_layout,
  Layout value_layout,
  Comparator comparator
);

void map_init_with(
  void* map,
  Layout key_layout,
  Layout value_layout,
  Comparator comparator,
  Allocator allocator
);

size_t map_count(const void* map);

void* map_get(const void* map, const void* key);

bool map_put(void* map, const void* key, const void* value);

bool map_remove(void* map, const void* key);

void map_check(const void* map);

#endif
