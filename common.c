#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "dynamicarrays.h"
#include "assembler.h"
#include "common.h"


bool streql(const char* cstr, const char* ptr, size_t len) {
  return strlen(cstr) == len && memcmp(cstr, ptr, len) == 0;
}

void report_error(size_t byte_pos, uint16_t size, char* comment, char* color, strview_t file, char* filename) {
  size_t row = 1, column = 1;
  char* line_start = file.ptr;
  size_t i, j;
  for (i = 0; i < byte_pos; ++i) {
    if (file.ptr[i] == '\n') {
      row += 1;
      column = 0;
      line_start = file.ptr + i;
    }
    column += 1;
  }

  for (j = i; j < file.size; ++j) {
    if (file.ptr[j] == '\n') break;
  }
  printf("%s:%zu:%zu: %s", filename, row, column, comment);
  printf("%.*s%s", (unsigned int) (i - (line_start - file.ptr)), line_start, color);
  printf("%.*s"CRESET, size, file.ptr + byte_pos);
  printf("%.*s\n", (unsigned int) (j - (byte_pos + size)), file.ptr + byte_pos + size);
  printf("%s%*s", color, (unsigned int) (i - (line_start - file.ptr)), "^");
  for (size_t i = 1; i < size; ++i) putc('~', stdout);
  printf(CRESET"\n");
}

void report_token_error(token_t token, char* comment, da_t files) {
  char buf[1024] = RED"error"CRESET": ";
  memcpy(buf + strlen(buf), comment, strlen(comment));

  size_t i = token.file_id;
  report_error(token.byte_pos, token.size, buf, RED, ((file_t*) files.items)[i].file, ((file_t*) files.items)[i].filename);
}

void report_token_warning(token_t token, char* comment, da_t files) {
  char buf[1024] = MAG"warning"CRESET": ";
  memcpy(buf + strlen(buf), comment, strlen(comment));

  size_t i = token.file_id;
  report_error(token.byte_pos, token.size, buf, MAG, ((file_t*) files.items)[i].file, ((file_t*) files.items)[i].filename);
}

