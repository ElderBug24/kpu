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

#define exit_error(comment) \
  do { \
    printf(RED "\nError: %s\n" CRESET, comment); \
    return 1; \
  } while (0);

