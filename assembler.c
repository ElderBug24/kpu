#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "dynamicarrays.h"
#include "assembler.h"
#include "common.h"


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

size_t tokenize_file(strview_t file, da_t* tokens, FILE_COUNT_T file_id) { // TODO: handle errors
  da_reserve(tokens, file.size / 8, sizeof(token_t));
  token_t token = {0};
  size_t token_start = 0;
  enum {
    COMMENT_NONE,
    COMMENT_MAYBE,
    COMMENT_YES
  } comment = COMMENT_NONE;
  enum {
    PARSING_NONE,
    PARSING_CHAR,
    PARSING_STRING,
    PARSING_NUMBER,
    PARSING_FLOAT,
    PARSING_IDENTIFIER
  } parsing, next_parsing;
  parsing = PARSING_NONE;
  next_parsing = PARSING_NONE;

  for (size_t i = 0; i < file.size; ++i) {
    char c = file.ptr[i];
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
    switch (c) {
      case '/':
        switch (comment) {
          case COMMENT_NONE:
            comment = COMMENT_MAYBE;
            next_parsing = PARSING_NONE;
            goto end_token;
          case COMMENT_MAYBE:
            comment = COMMENT_YES;
            break;
          case COMMENT_YES:
            break;
        }
        break;
      case '\n':
        comment = COMMENT_NONE;
        next_parsing = PARSING_NONE;
        goto end_token;
      case '"':
        token_start = i;
        next_parsing = PARSING_STRING;
        goto end_token;
      case '\'':
        token_start = i;
        next_parsing = PARSING_CHAR;
        goto end_token;
      case '(':
        token = (token_t) {
          .type = TOKEN_DELIMITER_PARENTHESIS,
          .value = {
            .TOKEN_DELIMITER_CLOSING = false
          },
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        next_parsing = PARSING_NONE;
        goto end_token;
      case ')':
        token = (token_t) {
          .type = TOKEN_DELIMITER_PARENTHESIS,
          .value = {
            .TOKEN_DELIMITER_CLOSING = true
          },
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        next_parsing = PARSING_NONE;
        goto end_token;
      case '[':
        token = (token_t) {
          .type = TOKEN_DELIMITER_BRACKET,
          .value = {
            .TOKEN_DELIMITER_CLOSING = false
          },
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        next_parsing = PARSING_NONE;
        goto end_token;
      case ']':
        token = (token_t) {
          .type = TOKEN_DELIMITER_BRACKET,
          .value = {
            .TOKEN_DELIMITER_CLOSING = true
          },
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        next_parsing = PARSING_NONE;
        goto end_token;
      case '{':
        token = (token_t) {
          .type = TOKEN_DELIMITER_BRACE,
          .value = {
            .TOKEN_DELIMITER_CLOSING = false
          },
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        next_parsing = PARSING_NONE;
        goto end_token;
      case '}':
        token = (token_t) {
          .type = TOKEN_DELIMITER_BRACE,
          .value = {
            .TOKEN_DELIMITER_CLOSING = true
          },
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        next_parsing = PARSING_NONE;
        goto end_token;
      case ',':
        token = (token_t) {
          .type = TOKEN_DELIMITER_COMMA,
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        next_parsing = PARSING_NONE;
        goto end_token;
      case ';':
        token = (token_t) {
          .type = TOKEN_DELIMITER_SEMICOLON,
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        next_parsing = PARSING_NONE;
        goto end_token;
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
        if (parsing != PARSING_NONE) {
          parsing = PARSING_NUMBER;
          token_start = i;
        }
        break;
      case '.':
        if (parsing == PARSING_NUMBER) parsing = PARSING_FLOAT;
        return i+1;
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
        next_parsing = PARSING_NONE;
        goto end_token;
    }

    continue;

  end_token:
    switch (parsing) {
      case PARSING_NONE:
        break;
      case PARSING_NUMBER:
        break;
      case PARSING_FLOAT:
        break;
      case PARSING_IDENTIFIER:
        if (streql("include", file.ptr + token_start, i - token_start)) {
          token = (token_t) {
            .type = TOKEN_KEYWORD_INCLUDE,
            .byte_pos = token_start,
            .size = i - token_start,
            .file_id = file_id
          };
          da_push(tokens, &token, sizeof(token_t));
          parsing = PARSING_NONE;
        } else if (streql("const", file.ptr + token_start, i - token_start)) {
          token = (token_t) {
            .type = TOKEN_KEYWORD_CONST,
            .byte_pos = token_start,
            .size = i - token_start,
            .file_id = file_id
          };
          da_push(tokens, &token, sizeof(token_t));
          parsing = PARSING_NONE;
        } else if (streql("fn", file.ptr + token_start, i - token_start)) {
          token = (token_t) {
            .type = TOKEN_KEYWORD_FN,
            .byte_pos = token_start,
            .size = i - token_start,
            .file_id = file_id
          };
          da_push(tokens, &token, sizeof(token_t));
          parsing = PARSING_NONE;
        } else if (streql("return", file.ptr + token_start, i - token_start)) {
          token = (token_t) {
            .type = TOKEN_KEYWORD_RETURN,
            .byte_pos = token_start,
            .size = i - token_start,
            .file_id = file_id
          };
          da_push(tokens, &token, sizeof(token_t));
          parsing = PARSING_NONE;
        } else if (streql("if", file.ptr + token_start, i - token_start)) {
          token = (token_t) {
            .type = TOKEN_KEYWORD_IF,
            .byte_pos = token_start,
            .size = i - token_start,
            .file_id = file_id
          };
          da_push(tokens, &token, sizeof(token_t));
          parsing = PARSING_NONE;
        } else if (streql("else", file.ptr + token_start, i - token_start)) {
          token = (token_t) {
            .type = TOKEN_KEYWORD_ELSE,
            .byte_pos = token_start,
            .size = i - token_start,
            .file_id = file_id
          };
          da_push(tokens, &token, sizeof(token_t));
          parsing = PARSING_NONE;
        } else {
          token = (token_t) {
            .type = TOKEN_IDENTIFIER,
            .byte_pos = token_start,
            .size = i - token_start,
            .file_id = file_id
          };
          da_push(tokens, &token, sizeof(token_t));
          parsing = PARSING_NONE;
        }
        break;
      default:
        exit_error("unreachable");
    }
    parsing = next_parsing;
  }

  return 0;
}

