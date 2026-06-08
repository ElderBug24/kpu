#define NOB_IMPLEMENTATION
#include "../nob.h"


int main(int argc, char** argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  Nob_Cmd cmd = {0};

  nob_mkdir_if_not_exists("build");
  nob_cmd_append(&cmd, "gcc", "-x", "c", "-Wall", "-Wextra", "-pedantic", "-o", "build/main", "main.c", "dynamicarrays.c", "common.c", "assembler.c");
  if (!nob_cmd_run(&cmd)) return 1;
  nob_cmd_append(&cmd, "build/main", "assemble", "example.kp");
  if (!nob_cmd_run(&cmd)) return 1;

  return 0;
}

