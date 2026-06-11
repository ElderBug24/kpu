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
void report_error(size_t, uint16_t, char*, char*, strview_t, char*);
void report_token_error(token_t, char*, da_t);
void report_token_warning(token_t, char*, da_t);


