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

size_t end_token(parsing_e parsing, da_t* tokens, size_t token_start, size_t i, strview_t file, uint16_t file_id) {
  token_t token;
  switch (parsing) {
    case PARSING_NONE:
      break;
    case PARSING_CHAR:
    case PARSING_STRING:
      return i+1;
    case PARSING_DASH:
      token = (token_t) {
        .type = TOKEN_OPERATOR_MINUS,
        .byte_pos = i-1,
        .size = 1,
        .file_id = file_id
      };
      da_push(tokens, &token, sizeof(token_t));
      break;
    case PARSING_SLASH:
      token = (token_t) {
        .type = TOKEN_OPERATOR_SLASH,
        .byte_pos = i-1,
        .size = 1,
        .file_id = file_id
      };
      da_push(tokens, &token, sizeof(token_t));
      break;
    case PARSING_ZERO:
      token = (token_t) {
        .type = TOKEN_LITERAL_NUM,
        .byte_pos = i-1,
        .size = 1,
        .file_id = file_id
      };
      da_push(tokens, &token, sizeof(token_t));
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
    case PARSING_NUMBER_BIN:
      token = (token_t) {
        .type = TOKEN_LITERAL_NUM_BIN,
        .byte_pos = token_start,
        .size = i - token_start,
        .file_id = file_id
      };
      da_push(tokens, &token, sizeof(token_t));
      break;
    case PARSING_NUMBER_HEX:
      token = (token_t) {
        .type = TOKEN_LITERAL_NUM_HEX,
        .byte_pos = token_start,
        .size = i - token_start,
        .file_id = file_id
      };
      da_push(tokens, &token, sizeof(token_t));
      break;
    case PARSING_NUMBER_OCT:
      token = (token_t) {
        .type = TOKEN_LITERAL_NUM_OCT,
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
    case PARSING_FLOAT_SCIENTIFIC:
      token = (token_t) {
        .type = TOKEN_LITERAL_FLOAT_SCIENTIFIC,
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

  return 0;
}

size_t tokenize_file(strview_t file, da_t* tokens, uint16_t file_id) {
  da_reserve(tokens, file.size / 8, sizeof(token_t));
  token_t token = {0};
  size_t token_start = 0;
  parsing_e parsing = PARSING_NONE;
  bool comment = false;
  bool escaped = false;
  size_t r = 0;

  size_t row = 1;
  size_t column = 0;
  for (size_t i = 0; i < file.size; ++i) {
    char c = file.ptr[i];
    if (c == '\r') {
      comment = false;
      r = end_token(parsing, tokens, token_start, i, file, file_id);
      if (r) return r;
      parsing = PARSING_NONE;
      continue;
    }
    if (c == '\n') {
      comment = false;
      row += 1;
      column = 0;
      r = end_token(parsing, tokens, token_start, i, file, file_id);
      if (r) return r;
      parsing = PARSING_NONE;
      continue;
    }
    column += 1;
    if (comment) continue;
    if (c == '\\' && (parsing == PARSING_STRING || parsing == PARSING_CHAR)) {
      escaped = true;
      continue;
    }
    if (parsing == PARSING_STRING) {
      if (c == '"' && !escaped) {
        token = (token_t) {
          .type = TOKEN_LITERAL_STR,
          .byte_pos = token_start,
          .size = i - token_start + 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        parsing = PARSING_NONE;
      }
      escaped = false;
      continue;
    }
    if (parsing == PARSING_CHAR) {
      if (c == '\'' && !escaped) {
        token = (token_t) {
          .type = TOKEN_LITERAL_CHAR,
          .byte_pos = token_start,
          .size = i - token_start + 1,
          .file_id = file_id
        };
        da_push(tokens, &token, sizeof(token_t));
        parsing = PARSING_NONE;
      }
      escaped = false;
      continue;
    }
    bool found = false;
    for (size_t j = 0; j < sizeof(DELIMITERS_LUT) / sizeof(DELIMITERS_LUT[0]); ++j) {
      if (c == DELIMITERS_LUT[j].delimiter) {
        token_t token = {
          .type = DELIMITERS_LUT[j].type,
          .byte_pos = i,
          .size = 1,
          .file_id = file_id
        };
        r = end_token(parsing, tokens, token_start, i, file, file_id);
        if (r) return r;
        parsing = PARSING_NONE;
        da_push(tokens, &token, sizeof(token_t));

        found = true;
        break;
      }
    }
    if (found) continue;
    switch (c) {
      case '/':
        if (parsing == PARSING_SLASH) {
          comment = true;
          parsing = PARSING_NONE;
          break;
        } else {
          parsing = PARSING_SLASH;
        }
        break;
      case '"':
        token_start = i;
        r = end_token(parsing, tokens, token_start, i, file, file_id);
        if (r) return r;
        parsing = PARSING_STRING;
        break;
      case '\'':
        token_start = i;
        r = end_token(parsing, tokens, token_start, i, file, file_id);
        if (r) return r;
        parsing = PARSING_CHAR;
        break;
      case '0':
        if (parsing == PARSING_NONE) {
          parsing = PARSING_ZERO;
          token_start = i;
        } else if (parsing == PARSING_DASH || parsing == PARSING_ZERO) {
          parsing = PARSING_NUMBER;
        }
        break;
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
        } else if (parsing == PARSING_DASH || parsing == PARSING_ZERO) {
          parsing = PARSING_NUMBER;
        }
        break;
      case '-':
        if (parsing == PARSING_NONE) {
          parsing = PARSING_DASH;
          token_start = i;
        } else if (parsing == PARSING_NUMBER || parsing == PARSING_FLOAT || parsing == PARSING_NUMBER_BIN || parsing == PARSING_NUMBER_HEX || parsing == PARSING_NUMBER_OCT) {
          return i+1;
        }
        break;
      case '>':
        if (parsing == PARSING_DASH) {
          token = (token_t) {
            .type = TOKEN_KEYWORD_ARROW,
            .byte_pos = i-1,
            .size = 2,
            .file_id = file_id
          };
          da_push(tokens, &token, sizeof(token_t));
          parsing = PARSING_NONE;
        } else {
          token = (token_t) {
            .type = TOKEN_DELIMITER_ANGLE_BRACKET,
            .byte_pos = i,
            .size = 1,
            .file_id = file_id
          };
          r = end_token(parsing, tokens, token_start, i, file, file_id);
          if (r) return r;
          da_push(tokens, &token, sizeof(token_t));
          parsing = PARSING_NONE;
        }
        break;
      case '.':
        if (parsing == PARSING_NUMBER) parsing = PARSING_FLOAT;
        else return i+1;
        break;
      case 'b':
      case 'B':
        if (parsing == PARSING_NONE) {
          parsing = PARSING_IDENTIFIER;
          token_start = i;
        } else if (parsing == PARSING_ZERO) {
          parsing = PARSING_NUMBER_BIN;
        }
        break;
      case 'x':
      case 'X':
        if (parsing == PARSING_NONE) {
          parsing = PARSING_IDENTIFIER;
          token_start = i;
        } else if (parsing == PARSING_ZERO) {
          parsing = PARSING_NUMBER_HEX;
        }
        break;
      case 'o':
      case 'O':
        if (parsing == PARSING_NONE) {
          parsing = PARSING_IDENTIFIER;
          token_start = i;
        } else if (parsing == PARSING_ZERO) {
          parsing = PARSING_NUMBER_OCT;
        }
        break;
      case 'e':
      case 'E':
        if (parsing == PARSING_NONE) {
          parsing = PARSING_IDENTIFIER;
          token_start = i;
        } else if (parsing == PARSING_NUMBER || parsing == PARSING_FLOAT) {
          parsing = PARSING_FLOAT_SCIENTIFIC;
        }
        break;
      case 'a':
      case 'c':
      case 'd':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'y':
      case 'z':
      case 'A':
      case 'C':
      case 'D':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
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
        r = end_token(parsing, tokens, token_start, i, file, file_id);
        if (r) return r;
        parsing = PARSING_NONE;
        break;
      default:
        return i+1;
    }
  }

  return 0;
}

