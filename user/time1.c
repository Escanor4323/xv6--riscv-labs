#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  if (argc < 2) {
    fprintf(2, "usage: time1 cmd [args...]\n");
    exit(1);
  }

  uint64 t0 = uptime();

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "time1: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    exec(argv[1], &argv[1]);
    fprintf(2, "time1: exec %s failed\n", argv[1]);
    exit(1);
  }

  int status = 0;
  wait(&status);

  uint64 t1 = uptime();
  printf("elapsed time: %lud ticks\n", t1 - t0);
  exit(0);
}

