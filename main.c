#define KPU_VERSION "0.0.0"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "dynamicarrays.h"
#include "assembler.h"
#include "common.h"


typedef enum {
  command_help,
  command_version,
  command_assemble
} command_e;

int main(int argc, char** argv) {
  if (argc < 2) exit_error("not enough arguments");

  command_e command;
  if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "h") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
    command = command_help;
  } else if (strcmp(argv[1], "version") == 0 || strcmp(argv[1], "v") == 0 || strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
    command = command_version;
  } else if (strcmp(argv[1], "assemble") == 0 || strcmp(argv[1], "ass") == 0 || strcmp(argv[1], "a") == 0) {
    command = command_assemble;
  } else exit_error("unkown command");

  switch (command) {
    case command_help:
      printf("usage: kpu <command> [args]\n");
      printf("\nAvailable commands:\n");
      printf("    help                        Print this help\n");
      printf("    version                     Print the program's version\n");
      printf("    assemble <file.kp>          Assemble a kp source file into a kpu executable\n");
      break;
    case command_version:
      printf("kpu version %s\n", KPU_VERSION);
      break;
    case command_assemble:
      if (argc < 3) exit_error("not enough arguments");
      char* filename = argv[2];

      printf("Assembling file '%s'...\n", filename);

      strview_t file = open_file(filename);
      if (!file.ptr) exit_error("could not open file '%s'", filename);

      da_t tokens = da_with_capacity(256, sizeof(token_t));
      size_t err = tokenize_file(file, &tokens, 0);
      if (err) exit_error("could not parse (byte %zu)", err - 1);

      printf("%zu tokens\n", tokens.count);
      size_t indent = 0;
      for (size_t i = 0; i < tokens.count; ++i) {
        token_t token = ((token_t*) tokens.items)[i];

        if (token.type == TOKEN_DELIMITER_BRACE) {
          if (token.value.TOKEN_DELIMITER_CLOSING) {
            indent -= 1;
            printf("%-*s", 2 * (unsigned int) indent + 1, "\r");
          } else {
            indent += 1;
          }
        }
        printf("%.*s ", (unsigned int) token.size, file.ptr + token.byte_pos);
        if (token.type == TOKEN_DELIMITER_SEMICOLON || token.type == TOKEN_DELIMITER_BRACE) {
          printf("%-*s", 2 * (unsigned int) indent + 1, "\n");
        }
      }

      printf("\n");

      free(file.ptr);
      da_destroy(tokens);

      break;
    default: exit_error("unreachable");
  }

  printf("\nExiting without errors...\n");
  return 0;
}

