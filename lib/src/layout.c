#include "layout.h"

Layout_builder layout_init(size_t max_alignment) {
  return (Layout_builder){
    .current_offset = 0,
    .current_size = 0,
    .current_alignment = 1,
    .max_alignment = max_alignment != 0 ? max_alignment : alignof(max_align_t),
  };
}

size_t layout_add(Layout_builder* builder, Layout layout) {
  size_t member_size = layout.size;
  size_t member_alignment = layout.alignment != 0 ? layout.alignment : 1;

  size_t member_offset = (builder->current_offset / member_alignment) * member_alignment;

  while (member_offset < builder->current_offset) {
    member_offset += member_alignment;
  }

  builder->current_offset = member_offset + member_size;

  if (member_alignment > builder->current_alignment) {
    size_t max_alignment =
      builder->max_alignment != 0 ? builder->max_alignment : alignof(max_align_t);

    builder->current_alignment =
      member_alignment <= max_alignment ? member_alignment : max_alignment;
  }

  return member_offset;
}

Layout layout_build(const Layout_builder* builder) {
  size_t max_alignment =
    builder->max_alignment != 0 ? builder->max_alignment : alignof(max_align_t);

  size_t size = (builder->current_offset / max_alignment) * max_alignment;

  while (size < builder->current_offset) {
    size += max_alignment;
  }

  return (Layout){
    .size = size,
    .alignment = builder->current_alignment != 0 ? builder->current_alignment : 1,
  };
}
