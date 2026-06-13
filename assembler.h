#include <stdint.h>


#define MAX_FILES UINT16_MAX

typedef void assemble_opt_t;

typedef struct {
  char* ptr;
  size_t size;
} strview_t;

typedef struct {
  strview_t file;
  char* filename;
} file_t;

typedef enum {
  TOKEN_IDENTIFIER,
  TOKEN_LITERAL_CHAR,
  TOKEN_LITERAL_STR,
  TOKEN_LITERAL_NUM,
  TOKEN_LITERAL_NUM_BIN,
  TOKEN_LITERAL_NUM_HEX,
  TOKEN_LITERAL_NUM_OCT,
  TOKEN_LITERAL_FLOAT,
  TOKEN_LITERAL_FLOAT_SCIENTIFIC,
  TOKEN_KEYWORD_INCLUDE,
  TOKEN_KEYWORD_CONST,
  TOKEN_KEYWORD_FN,
  TOKEN_KEYWORD_RETURN,
  TOKEN_KEYWORD_IF,
  TOKEN_KEYWORD_ELSE,
  TOKEN_KEYWORD_ARROW,
  TOKEN_DELIMITER_PARENTHESIS,
  TOKEN_DELIMITER_PARENTHESIS_CLOSING,
  TOKEN_DELIMITER_BRACKET,
  TOKEN_DELIMITER_BRACKET_CLOSING,
  TOKEN_DELIMITER_BRACE,
  TOKEN_DELIMITER_BRACE_CLOSING,
  TOKEN_DELIMITER_SQUARE_BRACKET,
  TOKEN_DELIMITER_SQUARE_BRACKET_CLOSING,
  TOKEN_DELIMITER_CURLY_BRACE,
  TOKEN_DELIMITER_CURLY_BRACE_CLOSING,
  TOKEN_DELIMITER_COMMA,
  TOKEN_DELIMITER_SEMICOLON,
  TOKEN_OPERATOR_EQUAL
} token_type_e;

typedef struct {
  size_t byte_pos;
  token_type_e type;
  uint16_t size;
  uint16_t file_id;
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
} DELIMITERS_LUT[] = {
  { '(', TOKEN_DELIMITER_PARENTHESIS            },
  { ')', TOKEN_DELIMITER_PARENTHESIS_CLOSING    },
  { '[', TOKEN_DELIMITER_SQUARE_BRACKET         },
  { ']', TOKEN_DELIMITER_SQUARE_BRACKET_CLOSING },
  { '{', TOKEN_DELIMITER_CURLY_BRACE            },
  { '}', TOKEN_DELIMITER_CURLY_BRACE_CLOSING    },
  { ',', TOKEN_DELIMITER_COMMA                  },
  { ';', TOKEN_DELIMITER_SEMICOLON              }
};

typedef enum {
  COMMENT_NONE,
  COMMENT_MAYBE,
  COMMENT_YES
} comment_e;

typedef enum {
  PARSING_NONE,
  PARSING_CHAR,
  PARSING_STRING,
  PARSING_ZERO,
  PARSING_NUMBER,
  PARSING_NUMBER_BIN,
  PARSING_NUMBER_HEX,
  PARSING_NUMBER_OCT,
  PARSING_FLOAT,
  PARSING_FLOAT_SCIENTIFIC,
  PARSING_IDENTIFIER,
  PARSING_DASH
} parsing_e;

static const unsigned char escape_lut[256] = {
  ['0']  = '\0',
  ['a']  = '\a',
  ['b']  = '\b',
  ['t']  = '\t',
  ['n']  = '\n',
  ['v']  = '\v',
  ['f']  = '\f',
  ['r']  = '\r',
  ['"']  = '\"',
  ['\''] = '\'',
  ['?']  = '\?',
  ['\\'] = '\\',
};

strview_t open_file(char*);
size_t tokenize_file(strview_t, da_t*, uint16_t);

