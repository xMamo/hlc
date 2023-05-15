#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#include "allocator.h"
#include "iterable.h"
#include "iterator.h"
#include "layout.h"

extern const Layout darray_layout;

extern const Iterable_methods darray_iterable_methods;

void darray_init(void* darray, Layout item_layout);

void darray_init_with(void* darray, Layout item_layout, Allocator allocator);

void darray_destroy(void* darray);

size_t darray_count(const void* darray);

size_t darray_capacity(const void* darray);

Layout darray_item_layout(const void* darray);

Allocator darray_allocator(const void* darray);

bool darray_reserve(void* darray, size_t count);

void darray_fit(void* darray);

bool darray_insert(void* darray, size_t index, const void* item);

bool darray_insert_many(void* darray, size_t index, const void* items, size_t count);

void darray_remove(void* darray, size_t index);

void darray_remove_many(void* darray, size_t index, size_t count);

bool darray_push(void* darray, const void* item);

bool darray_pop(void* darray, void* out_item);

void* darray_head(const void* darray);

void* darray_at(const void* darray, size_t index);

Layout darray_iterator_layout(const void* darray);

void darray_iterator(const void* darray, void* out_iterator);

void* darray_iterator_next(void* iterator);

void darray_iterator_skip(void* iterator, size_t count);

#endif
