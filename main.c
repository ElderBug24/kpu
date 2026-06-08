#define KPU_VERSION "0.0.0"

#include <stdio.h>
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
      for (size_t i = 0; i < tokens.count; ++i) {
        token_t token = ((token_t*) tokens.items)[i];
        switch (token.type) {
          case TOKEN_IDENTIFIER:
            printf("TOKEN_IDENTIFIER\n");
            break;
          case TOKEN_LITERAL_CHAR:
            printf("TOKEN_LITERAL_CHAR\n");
            break;
          case TOKEN_LITERAL_STR:
            printf("TOKEN_LITERAL_STR\n");
            break;
          case TOKEN_LITERAL_U8:
            printf("TOKEN_LITERAL_U8\n");
            break;
          case TOKEN_LITERAL_U16:
            printf("TOKEN_LITERAL_U16\n");
            break;
          case TOKEN_LIRERAL_U32:
            printf("TOKEN_LIRERAL_U32\n");
            break;
          case TOKEN_LIRERAL_U64:
            printf("TOKEN_LIRERAL_U64\n");
            break;
          case TOKEN_LIRERAL_USIZE:
            printf("TOKEN_LIRERAL_USIZE\n");
            break;
          case TOKEN_LITERAL_I8:
            printf("TOKEN_LITERAL_I8\n");
            break;
          case TOKEN_LITERAL_I16:
            printf("TOKEN_LITERAL_I16\n");
            break;
          case TOKEN_LIRERAL_I32:
            printf("TOKEN_LIRERAL_I32\n");
            break;
          case TOKEN_LIRERAL_I64:
            printf("TOKEN_LIRERAL_I64\n");
            break;
          case TOKEN_LIRERAL_ISIZE:
            printf("TOKEN_LIRERAL_ISIZE\n");
            break;
          case TOKEN_LITERAL_FLOAT:
            printf("TOKEN_LITERAL_FLOAT\n");
            break;
          case TOKEN_LITERAL_DOUBLE:
            printf("TOKEN_LITERAL_DOUBLE\n");
            break;
          case TOKEN_KEYWORD_INCLUDE:
            printf("TOKEN_KEYWORD_INCLUDE\n");
            break;
          case TOKEN_KEYWORD_CONST:
            printf("TOKEN_KEYWORD_CONST\n");
            break;
          case TOKEN_KEYWORD_FN:
            printf("TOKEN_KEYWORD_FN\n");
            break;
          case TOKEN_KEYWORD_RETURN:
            printf("TOKEN_KEYWORD_RETURN\n");
            break;
          case TOKEN_KEYWORD_IF:
            printf("TOKEN_KEYWORD_IF\n");
            break;
          case TOKEN_KEYWORD_ELSE:
            printf("TOKEN_KEYWORD_ELSE\n");
            break;
          case TOKEN_DELIMITER_PARENTHESIS:
            printf("TOKEN_DELIMITER_PARENTHESIS %s \n", token.value.TOKEN_DELIMITER_CLOSING ? "closing" : "");
            break;
          case TOKEN_DELIMITER_BRACE:
            printf("TOKEN_DELIMITER_BRACE %s \n", token.value.TOKEN_DELIMITER_CLOSING ? "closing" : "");
            break;
          case TOKEN_DELIMITER_BRACKET:
            printf("TOKEN_DELIMITER_BRACKET %s \n", token.value.TOKEN_DELIMITER_CLOSING ? "closing" : "");
          break;
          case TOKEN_DELIMITER_COMMA:
            printf("TOKEN_DELIMITER_COMMA\n");
            break;
          case TOKEN_DELIMITER_SEMICOLON:
            printf("TOKEN_DELIMITER_SEMICOLON\n");
            break;
          case TOKEN_OPERATOR_EQUAL:
          printf("TOKEN_OPERATOR_EQUAL\n");
          break;
        }
      }

      free(file.ptr);
      da_destroy(tokens);

      break;
    default: exit_error("unreachable");
  }

  printf("\nExiting without errors...\n");
  return 0;
}

