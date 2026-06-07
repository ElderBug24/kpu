#include <stdint.h>


typedef struct {
  void* items;
  size_t count;
  size_t capacity;
} da_t;

da_t da_with_capacity(size_t, size_t);
void da_destroy(da_t);
void da_reserve_exact(da_t*, size_t, size_t);
void da_reserve(da_t*, size_t, size_t);
void da_push(da_t*, void*, size_t);
void da_push_many(da_t* restrict, void* restrict, size_t, size_t);
void* da_pop(da_t*, size_t);
void da_swap_remove(da_t*, size_t, size_t);
void da_remove(da_t*, size_t, size_t);

