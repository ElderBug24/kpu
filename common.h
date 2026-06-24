#include <stdio.h>


#define BLK    "\033[0;30m"
#define RED    "\033[0;31m"
#define GRN    "\033[0;32m"
#define YEL    "\033[0;33m"
#define BLU    "\033[0;34m"
#define MAG    "\033[0;35m"
#define CYN    "\033[0;36m"
#define WHT    "\033[0;37m"
#define CRESET "\033[0m"

#define exit_error(...) \
  do { \
    printf(RED "\nError: "); \
    printf(__VA_ARGS__); \
    printf(CRESET "\n"); \
    return 1; \
  } while (0);

bool streql(const char*, const char*, size_t);
void report_error(size_t, uint16_t, char*, strview_t, char*, char*);
void report_token_error(token_t, char*, da_t);
void report_token_warning(token_t, char*, da_t);

#define report_error_va(byte_pos, size_, color, file, filename, ...) \
 do { \
    size_t row = 1, column = 1; \
    char* line_start = (file).ptr; \
    size_t i, j; \
    for (i = 0; i < (byte_pos); ++i) { \
      if ((file).ptr[i] == '\n') { \
        row += 1; \
        column = 0; \
        line_start = (file).ptr + i; \
      } \
      column += 1; \
    } \
    for (j = i; j < (file).size; ++j) { \
      if ((file).ptr[j] == '\n') break;\
    } \
    printf("%s:%zu:%zu: ", (filename), row, column); \
    printf(__VA_ARGS__); \
    printf("%.*s%s", (unsigned int) (i - (line_start - (file).ptr)), line_start, (color)); \
    printf("%.*s"CRESET, (size_), (file).ptr + (byte_pos)); \
    printf("%.*s\n", (unsigned int) (j - ((byte_pos) + (size_))), (file).ptr + (byte_pos) + (size_)); \
    printf("%s%*s", (color), (unsigned int) (i - (line_start - (file).ptr)), "^"); \
    for (size_t i = 1; i < (size_); ++i) putc('~', stdout); \
    printf(CRESET"\n"); \
  } while (0);

