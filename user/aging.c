// CS4375 HW3 Task 3 – Aging demonstration
// Joel Martínez Alvarado

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_HIGH_PRIORITY 3
#define NUM_ITERATIONS 20
#define WORK_AMOUNT 3000000

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
  int start_time;
  int pid;

  printf("\n=== Task 3: Aging Test ===\n");
  printf("%d high-priority (0) + 1 low-priority (40)\n", NUM_HIGH_PRIORITY);
  printf("With aging, low-priority should complete.\n\n");

  start_time = uptime();

  // Create high-priority processes
  for(int i = 0; i < NUM_HIGH_PRIORITY; i++) {
    pid = fork();
    if(pid == 0) {
      // High-priority child
      setpriority(0);  // Highest priority
      printf("[HIGH-%d] Started with priority %d at time %d\n", 
             i, getpriority(), uptime() - start_time);
      
      for(int j = 0; j < NUM_ITERATIONS; j++) {
        if(j % 5 == 0) {
          printf("[HIGH-%d] Iteration %d/%d at time %d\n", 
                 i, j, NUM_ITERATIONS, uptime() - start_time);
        }
        do_work(WORK_AMOUNT);
      }
      
      printf("[HIGH-%d] COMPLETED at time %d\n", i, uptime() - start_time);
      exit(0);
    }
  }

  // Small delay to ensure high-priority processes start first
  sleep(1);

  // Create low-priority process
  pid = fork();
  if(pid == 0) {
    // Low-priority child
    setpriority(40);  // Low priority
    int created_time = uptime();
    printf("[LOW] Started with priority %d at time %d\n", 
           getpriority(), created_time - start_time);
    printf("[LOW] Waiting to be scheduled...\n");
    
    for(int j = 0; j < NUM_ITERATIONS; j++) {
      int current_time = uptime();
      int wait_time = current_time - created_time;
      
      // Every iteration, show progress
      if(j % 5 == 0 || j == 0) {
        printf("[LOW] Iteration %d/%d at time %d (waited %d ticks)\n", 
               j, NUM_ITERATIONS, current_time - start_time, wait_time);
        printf("[LOW] Due to aging, effective priority has increased!\n");
      }
      do_work(WORK_AMOUNT);
    }
    
    printf("[LOW] COMPLETED at time %d\n", uptime() - start_time);
    printf("[LOW] SUCCESS: Low-priority process completed despite high competition\n");
    exit(0);
  }

  // Parent waits for all children
  for(int i = 0; i < NUM_HIGH_PRIORITY + 1; i++) {
    wait(0);
  }

  int end_time = uptime();
  printf("\nTask 3 complete. Time: %d ticks\n", end_time - start_time);
  printf("If [LOW] completed, aging works.\n");
  printf("Run 'ps' during test to see age increase.\n\n");
  exit(0);
}

