#ifndef LAYOUT_H
#define LAYOUT_H

#include <stdalign.h>
#include <stddef.h>

typedef struct Layout {
  size_t size;
  size_t alignment;
} Layout;

typedef struct Layout_builder {
  size_t current_offset;
  size_t current_size;
  size_t current_alignment;
  size_t max_alignment;
} Layout_builder;

#define LAYOUT(T) ((Layout){.size = sizeof(T), .alignment = alignof(T)})

Layout_builder layout_init(size_t max_alignment);

size_t layout_add(Layout_builder* builder, Layout layout);

Layout layout_build(const Layout_builder* builder);

#endif
