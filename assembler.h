#include <stdint.h>

#define ALLOC_ERROR 2
#define FILE_ERROR 3

#define MAX_FILES 255
typedef uint8_t FILE_COUNT_T;

typedef struct {} assemble_opt_t;

int assemble_file(char*, assemble_opt_t);
int assemble(char*, size_t, assemble_opt_t);

typedef enum {
  KEYWORD_INCLUDE,
  KEYWORD_CONST,
  KEYWORD_FN,
  KEYWORD_RETURN,
  KEYWORD_IF,
  KEYWORD_ELSE
} token_keyword_e;
typedef struct {
  union {
    char LITERAL_CHAR;
    uint8_t LITERAL_U8;
    uint16_t LITERAL_U16;
    uint32_t LIRERAL_U32;
    int8_t LITERAL_I8;
    int16_t LITERAL_I16;
    int32_t LIRERAL_I32;
    float LITERAL_FLOAT;
    double LITERAL_DOUBLE;
  } value;
  enum {
    LITERAL_CHAR,
    LITERAL_STR,
    LITERAL_U8,
    LITERAL_U16,
    LIRERAL_U32,
    LITERAL_I8,
    LITERAL_I16,
    LIRERAL_I32,
    LITERAL_FLOAT,
    LITERAL_DOUBLE
  } type;
} token_literal_t;
typedef struct {
  enum {
    DELIMITER_PARENTHESIS,
    DELIMITER_BRACE,
    DELIMITER_BRACKET,
    DELIMITER_COMMA,
    DELIMITER_SEMICOLON
  } type;
  bool closing;
} token_delimiter_t;

typedef struct {
  union {
    token_literal_t TOKEN_LITERAL;
    token_delimiter_t TOKEN_DELIMITER;
    token_keyword_e TOKEN_KEYWORD;
  } value;
  size_t byte;
  size_t size;
  enum {
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_LITERAL,
    TOKEN_DELIMITER
  } type;
  FILE_COUNT_T file_id;
} token_t;

// typedef struct {
//   union {
//
//   } value;
//   enum {
//
//   } type;
// } lex_token_t;

