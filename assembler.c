#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "dynamicarrays.h"
#include "assembler.h"


strview_t open_file(char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) return (strview_t) {0};
  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char* buffer = malloc(file_size + 1);
  if (!buffer) {
    fclose(file);

    return (strview_t) {0};
  }
  fread(buffer, 1, file_size, file);
  buffer[file_size] = 0;
  fclose(file);

  return (strview_t) {
    .ptr = buffer,
    .size = file_size
  };
}

da_t tokenize_file(strview_t file) {
  da_t tokens = da_with_capacity(file.size / 8, sizeof(token_t));

  return tokens;
}

