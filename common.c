#include <string.h>
#include <stdbool.h>


bool streql(const char* cstr, const char* ptr, size_t len) {
  return strlen(cstr) == len && memcmp(cstr, ptr, len) == 0;
}
