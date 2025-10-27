#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Test program for priority scheduler with aging

int
main(int argc, char *argv[])
{
  int pid;
  int i;

  printf("Priority Scheduler Test\n");
  printf("========================\n\n");

  // Test 1: Basic priority setting
  printf("Test 1: Setting and getting priority\n");
  printf("Current priority: %d\n", getpriority());
  
  if(setpriority(25) < 0) {
    printf("Failed to set priority\n");
    exit(1);
  }
  printf("After setpriority(25): %d\n", getpriority());
  
  if(setpriority(0) < 0) {
    printf("Failed to set priority\n");
    exit(1);
  }
  printf("After setpriority(0): %d\n\n", getpriority());

  // Test 2: Priority inheritance
  printf("Test 2: Priority inheritance (parent=10)\n");
  if(setpriority(10) < 0) {
    printf("Failed to set priority\n");
    exit(1);
  }
  
  pid = fork();
  if(pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  
  if(pid == 0) {
    // Child process
    printf("Child process priority: %d (should be 10)\n", getpriority());
    exit(0);
  } else {
    // Parent process
    wait(0);
  }

  // Test 3: Multiple processes with different priorities
  printf("\nTest 3: Multiple processes with different priorities\n");
  
  for(i = 0; i < 3; i++) {
    pid = fork();
    if(pid < 0) {
      printf("fork failed\n");
      exit(1);
    }
    
    if(pid == 0) {
      // Child process
      int my_priority = i * 15;  // 0, 15, 30
      setpriority(my_priority);
      printf("Child %d: priority=%d, doing work...\n", getpid(), my_priority);
      
      // Do some work
      int count = 0;
      for(int j = 0; j < 1000000; j++) {
        count++;
      }
      
      printf("Child %d: finished work (count=%d)\n", getpid(), count);
      exit(0);
    }
  }
  
  // Wait for all children
  for(i = 0; i < 3; i++) {
    wait(0);
  }

  printf("\nAll tests completed!\n");
  exit(0);
}

