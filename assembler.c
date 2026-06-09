#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "dynamicarrays.h"
#include "assembler.h"
#include "common.h"


strview_t open_file(char* filename) {
  FILE* file = fopen(filename, "rb");
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

void end_token(parsing_e* parsing, parsing_e next_parsing, da_t* tokens, size_t token_start, size_t i, strview_t file, FILE_COUNT_T file_id) {
  token_t token;
  switch (*parsing) {
    case PARSING_NONE:
    case PARSING_CHAR:
    case PARSING_STRING:
      break;
    case PARSING_NUMBER:
      token = (token_t) {
        .type = TOKEN_LITERAL_NUM,
        .byte_pos = token_start,
        .size = i - token_start,
        .file_id = file_id
      };
      da_push(tokens, &token, sizeof(token_t));
      break;
    case PARSING_FLOAT:
      token = (token_t) {
        .type = TOKEN_LITERAL_FLOAT,
        .byte_pos = token_start,
        .size = i - token_start,
        .file_id = file_id
      };
      da_push(tokens, &token, sizeof(token_t));
      break;
    case PARSING_IDENTIFIER:
      bool found = false;
      for (size_t j = 0; j < sizeof(KEYWORDS_LUT) / sizeof(KEYWORDS_LUT[0]); ++j) {
        if (streql(KEYWORDS_LUT[j].keyword, file.ptr + token_start, i - token_start)) {
          token_t token = {
            .type = KEYWORDS_LUT[j].type,
            .byte_pos = token_start,
            .size = i - token_start,
            .file_id = file_id
          };
          da_push(tokens, &token, sizeof(token_t));

          found = true;
          break;
        }
      }
      if (!found) {
        token = (token_t) {
          .type = TOKEN_IDENTIFIER,
          .byte_pos = token_start,
          .size = i - token_start,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
      }
      break;
  }
  *parsing = next_parsing;
}

size_t tokenize_file(strview_t file, da_t* tokens, FILE_COUNT_T file_id) {
  da_reserve(tokens, file.size / 8, sizeof(token_t));
  token_t token = {0};
  size_t token_start = 0;
  comment_e comment = COMMENT_NONE;
  parsing_e parsing;
  parsing = PARSING_NONE;

  size_t row = 1;
  size_t column = 0;
  for (size_t i = 0; i < file.size; ++i) {
    char c = file.ptr[i];
    if (c == '\r') {
      if (comment == COMMENT_YES) comment = COMMENT_NONE;
      end_token(&parsing, PARSING_NONE, tokens, token_start, i, file, file_id);
      continue;
    }
    if (c == '\n') {
      if (comment == COMMENT_YES) comment = COMMENT_NONE;
      row += 1;
      column = 0;
      end_token(&parsing, PARSING_NONE, tokens, token_start, i, file, file_id);
      continue;
    }
    column += 1;
    if (comment == COMMENT_YES) continue;
    if (comment == COMMENT_MAYBE && c != '/') return i+1;
    if (parsing == PARSING_STRING) {
      if (c == '"') {
        token = (token_t) {
          .type = TOKEN_LITERAL_STR,
          .value = {
            .TOKEN_STR_DELIMITERS_SIZE = 1
          },
          .byte_pos = token_start,
          .size = i - token_start + 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        parsing = PARSING_NONE;
      }
      continue;
    }
    if (parsing == PARSING_CHAR) {
      if (c == '\'') {
        token = (token_t) {
          .type = TOKEN_LITERAL_CHAR,
          .value = {
            .TOKEN_LITERAL_CHAR = file.ptr[i-1]
          },
          .byte_pos = token_start,
          .size = 3,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        parsing = PARSING_NONE;
      }
      continue;
    }
    bool found = false;
    for (size_t j = 0; j < sizeof(DELIMITERS_LUT) / sizeof(DELIMITERS_LUT[0]); ++j) {
      if (c == DELIMITERS_LUT[j].delimiter) {
        token_t token = {
          .type = DELIMITERS_LUT[j].type,
          .value = {
            .TOKEN_DELIMITER_CLOSING = DELIMITERS_LUT[j].closing
          },
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        end_token(&parsing, PARSING_NONE, tokens, token_start, i, file, file_id);
        da_push(tokens, &token, sizeof(token_t));

        found = true;
        break;
      }
    }
    if (found) continue;
    switch (c) {
      case '/':
        switch (comment) {
          case COMMENT_NONE:
            comment = COMMENT_MAYBE;
            end_token(&parsing, PARSING_NONE, tokens, token_start, i, file, file_id);
            break;
          case COMMENT_MAYBE:
            comment = COMMENT_YES;
            break;
          case COMMENT_YES:
            break;
        }
        break;
      case '"':
        token_start = i;
        end_token(&parsing, PARSING_STRING, tokens, token_start, i, file, file_id);
        break;
      case '\'':
        token_start = i;
        end_token(&parsing, PARSING_CHAR, tokens, token_start, i, file, file_id);
        break;
      case '=':
        token = (token_t) {
          .type = TOKEN_OPERATOR_EQUAL,
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        end_token(&parsing, PARSING_NONE, tokens, token_start, i, file, file_id);
        da_push(tokens, &token, sizeof(token_t));
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (parsing == PARSING_NONE) {
          parsing = PARSING_NUMBER;
          token_start = i;
        }
        break;
      case '.':
        if (parsing == PARSING_NUMBER) parsing = PARSING_FLOAT;
        else return i+1;
        break;
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
      case '_':
        if (parsing == PARSING_NONE) {
          parsing = PARSING_IDENTIFIER;
          token_start = i;
        }
        break;
      case ' ':
      case '\t':
        end_token(&parsing, PARSING_NONE, tokens, token_start, i, file, file_id);
        break;
    }
  }

  return 0;
}

