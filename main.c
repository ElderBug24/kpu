#define KPU_VERSION "0.0.0"

#include <stdio.h>
#include <string.h>

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
      break;
    default: exit_error("unreachable");
  }

  return 0;
}

