#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct rusage {
  uint cputime;
};

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
  struct rusage ru;
  int r = wait2(&status, &ru);
  if(r < 0){
    wait(&status);
    uint64 tf = uptime();
    printf("elapsed time: %lud ticks\n", tf - t0);
    exit(0);
  }

  uint64 t1 = uptime();
  uint64 elapsed = t1 - t0;
  uint64 cpu = ru.cputime;
  int percent = (elapsed == 0) ? 0 : (int)((cpu * 100) / elapsed);

  printf("elapsed time: %lud ticks , cpu time: %lud ticks , %d%% CPU\n",
         elapsed, cpu, percent);
  exit(0);
}

