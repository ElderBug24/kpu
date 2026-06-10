#include <stdint.h>


#define MAX_FILES 255
typedef uint8_t FILE_COUNT_T;

typedef void assemble_opt_t;

typedef struct {
  char* ptr;
  size_t size;
} strview_t;

typedef enum {
  TOKEN_IDENTIFIER,
  TOKEN_LITERAL_CHAR,
  TOKEN_LITERAL_STR,
  TOKEN_LITERAL_NUM,
  TOKEN_LITERAL_FLOAT,
  TOKEN_KEYWORD_INCLUDE,
  TOKEN_KEYWORD_CONST,
  TOKEN_KEYWORD_FN,
  TOKEN_KEYWORD_RETURN,
  TOKEN_KEYWORD_IF,
  TOKEN_KEYWORD_ELSE,
  TOKEN_KEYWORD_ARROW,
  TOKEN_DELIMITER_PARENTHESIS,
  TOKEN_DELIMITER_BRACE,
  TOKEN_DELIMITER_BRACKET,
  TOKEN_DELIMITER_COMMA,
  TOKEN_DELIMITER_SEMICOLON,
  TOKEN_OPERATOR_EQUAL
} token_type_e;

typedef struct {
  size_t byte_pos;
  size_t size;
  FILE_COUNT_T file_id;
  union {
    uint8_t TOKEN_STR_DELIMITERS_SIZE;
    bool TOKEN_DELIMITER_CLOSING;
  } value;
  token_type_e type;
} token_t;

static const struct {
  const char* keyword;
  token_type_e type;
} KEYWORDS_LUT[] = {
  { "include", TOKEN_KEYWORD_INCLUDE },
  { "const",   TOKEN_KEYWORD_CONST   },
  { "fn",      TOKEN_KEYWORD_FN      },
  { "return",  TOKEN_KEYWORD_RETURN  },
  { "if",      TOKEN_KEYWORD_IF      },
  { "else",    TOKEN_KEYWORD_ELSE    }
};

static const struct {
  const char delimiter;
  token_type_e type;
  bool closing;
} DELIMITERS_LUT[] = {
  { '(',       TOKEN_DELIMITER_PARENTHESIS, false },
  { ')',       TOKEN_DELIMITER_PARENTHESIS, true  },
  { '[',       TOKEN_DELIMITER_BRACKET,     false },
  { ']',       TOKEN_DELIMITER_BRACKET,     true  },
  { '{',       TOKEN_DELIMITER_BRACE,       false },
  { '}',       TOKEN_DELIMITER_BRACE,       true  },
  { ',',       TOKEN_DELIMITER_COMMA,       false },
  { ';',       TOKEN_DELIMITER_SEMICOLON,   false }
};

strview_t open_file(char*);
size_t tokenize_file(strview_t, da_t*, FILE_COUNT_T);

typedef enum {
  COMMENT_NONE,
  COMMENT_MAYBE,
  COMMENT_YES
} comment_e;

typedef enum {
  PARSING_NONE,
  PARSING_CHAR,
  PARSING_STRING,
  PARSING_NUMBER,
  PARSING_FLOAT,
  PARSING_IDENTIFIER,
  PARSING_DASH
} parsing_e;

