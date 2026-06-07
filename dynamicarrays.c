#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dynamicarrays.h"


da_t da_with_capacity(size_t capacity, size_t size) {
  return (da_t) {
    .items = malloc(capacity * size),
    .count = 0,
    .capacity = capacity
  };
}

void da_destroy(da_t da) {
  free(da.items);
}

void da_reserve_exact(da_t* da, size_t capacity, size_t size) {
  if (capacity > da->capacity) {
    da->items = realloc(da->items, capacity * size);
    da->capacity = capacity;
  }
}

void da_reserve(da_t* da, size_t capacity, size_t size) {
  if (capacity == da->capacity) return;
  size_t new_capacity = da->capacity;
  while (new_capacity < capacity) new_capacity *= 2;

  da_reserve_exact(da, new_capacity, size);
}

void da_push(da_t* restrict da, void* restrict item, size_t size) {
  da_reserve(da, da->count+1, size);
  memcpy(&((uint8_t*) da->items)[da->count * size], item, size);
  da->count += 1;
}

void da_push_many(da_t* restrict da, void* restrict items, size_t count, size_t size) {
  da_reserve(da, da->count+count, size);
  memcpy(&((uint8_t*) da->items)[da->count * size], items, count * size);
  da->count += count;
}

void* da_pop(da_t* da, size_t size) {
  return &((uint8_t*) da->items)[(--da->count) * size];
}

void da_swap_remove(da_t* da, size_t index, size_t size) {
  memcpy(&((uint8_t*) da->items)[index * size], &((uint8_t*) da->items)[(--da->count) * size], size);
}

void da_remove(da_t* da, size_t index, size_t size) {
  memmove(&((uint8_t*) da->items)[index * size], &((uint8_t*) da->items)[(index + 1) * size], (--da->count - index) * size);
}

