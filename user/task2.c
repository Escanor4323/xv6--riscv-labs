// CS4375 HW3 Task 2 – Scheduling order test
// Joel Martínez Alvarado

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_ITERATIONS 10
#define WORK_AMOUNT 5000000

// Do some CPU work
void
do_work(int amount)
{
  volatile int count = 0;
  for(int i = 0; i < amount; i++) {
    count++;
  }
}

int
main(int argc, char *argv[])
{
  int pid_a, pid_b, pid_c;
  int start_time;

  printf("\n=== Task 2: Scheduling Order ===\n");
  printf("Process A: priority 40\n");
  printf("Process B: priority 39\n");
  printf("Process C: priority 40\n");
  printf("Expected: B runs less\n\n");

  start_time = uptime();

  // Create Process A with priority 40
  pid_a = fork();
  if(pid_a == 0) {
    // Process A
    setpriority(40);
    printf("[A] Started with priority %d at time %d\n", getpriority(), uptime() - start_time);
    
    for(int i = 0; i < NUM_ITERATIONS; i++) {
      printf("[A] Iteration %d at time %d (priority %d)\n", 
             i, uptime() - start_time, getpriority());
      do_work(WORK_AMOUNT);
    }
    
    printf("[A] Finished at time %d\n", uptime() - start_time);
    exit(0);
  }

  // Create Process B with priority 39 (lower)
  pid_b = fork();
  if(pid_b == 0) {
    // Process B
    setpriority(39);
    printf("[B] Started with priority %d at time %d\n", getpriority(), uptime() - start_time);
    
    for(int i = 0; i < NUM_ITERATIONS; i++) {
      printf("[B] Iteration %d at time %d (priority %d)\n", 
             i, uptime() - start_time, getpriority());
      do_work(WORK_AMOUNT);
    }
    
    printf("[B] Finished at time %d\n", uptime() - start_time);
    exit(0);
  }

  // Create Process C with priority 40
  pid_c = fork();
  if(pid_c == 0) {
    // Process C
    setpriority(40);
    printf("[C] Started with priority %d at time %d\n", getpriority(), uptime() - start_time);
    
    for(int i = 0; i < NUM_ITERATIONS; i++) {
      printf("[C] Iteration %d at time %d (priority %d)\n", 
             i, uptime() - start_time, getpriority());
      do_work(WORK_AMOUNT);
    }
    
    printf("[C] Finished at time %d\n", uptime() - start_time);
    exit(0);
  }

  // Parent waits for all children
  wait(0);
  wait(0);
  wait(0);

  printf("\nTask 2 complete.\n");
  printf("Check timestamps - B should run less than A and C.\n\n");

  exit(0);
}

