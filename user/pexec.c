// CS4375 HW3 Task 4 – Execute at priority
// Joel Martínez Alvarado
// Usage: pexec <priority> <command> [args...]

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int priority;
  int pid;

  if(argc < 3) {
    printf("Usage: pexec <priority> <command> [args...]\n");
    printf("Priority: 0-49\n");
    exit(1);
  }

  priority = atoi(argv[1]);

  // Validate priority
  if(priority < 0 || priority > 49) {
    printf("Error: Priority must be between 0 and 49\n");
    exit(1);
  }

  if(setpriority(priority) < 0) {
    printf("setpriority failed\n");
    exit(1);
  }

  printf("pexec: Running %s at priority %d\n", argv[2], priority);
  printf("------------------------------------------------------\n");

  // Fork and exec the command
  pid = fork();
  if(pid < 0) {
    printf("Error: fork failed\n");
    exit(1);
  }

  if(pid == 0) {
    // Child: exec the command
    // Child inherits parent's priority
    exec(argv[2], argv + 2);
    printf("Error: exec %s failed\n", argv[2]);
    exit(1);
  }

  int start_time = uptime();
  wait(0);
  int end_time = uptime();

  printf("------------------------------------------------------\n");
  printf("pexec: Done. Time: %d ticks (priority %d)\n", end_time - start_time, priority);

  exit(0);
}

