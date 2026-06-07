#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "assembler.h"


int assemble_file(char* filename, assemble_opt_t opt) {
  FILE* file = fopen(filename, "r");
  if (!file) return FILE_ERROR;
  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char* buffer = malloc(file_size + 1);
  if (!buffer) {
    fclose(file);
    return ALLOC_ERROR;
  }
  fread(buffer, 1, file_size, file);
  buffer[file_size] = 0;
  fclose(file);

  int ret = assemble(buffer, file_size, opt);

  free(buffer);

  return ret;
}

int assemble(char* buffer, size_t file_size, assemble_opt_t opt) {
  (void) buffer;
  (void) file_size;
  (void) opt;

  token_t token = {
    .byte = 84,
    .size = 3,
    .file_id = 0,
    .value = {
      .TOKEN_LITERAL = (token_literal_t) {
        .type = LITERAL_CHAR,
        .value = {
          .LITERAL_CHAR = '@'
        }
      }
    },
    .type = TOKEN_LITERAL,
  };

  (void) token;

  return 0;
}

