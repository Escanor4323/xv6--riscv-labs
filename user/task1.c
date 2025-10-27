// CS4375 HW3 Task 1 – Priority syscalls and inheritance
// Joel Martínez Alvarado

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  int initial_priority, parent_priority, child_priority;

  printf("\n=== Task 1: Priority System Calls ===\n\n");

  // test initial priority
  printf("Test 1: Initial priority\n");
  initial_priority = getpriority();
  printf("  Initial: %d (should be 0)\n", initial_priority);
  if(initial_priority != 0) {
    printf("  FAIL\n");
  } else {
    printf("  PASS\n");
  }
  printf("\n");

  // test set/get
  printf("Test 2: Set and get\n");
  if(setpriority(10) < 0) {
    printf("  FAIL\n");
  } else {
    int new_priority = getpriority();
    printf("  After setpriority(10): %d\n", new_priority);
    if(new_priority == 10) {
      printf("  PASS\n");
    } else {
      printf("  FAIL\n");
    }
  }
  printf("\n");

  // test range validation
  printf("Test 3: Range validation\n");
  if(setpriority(-1) < 0) {
    printf("  -1 rejected: PASS\n");
  } else {
    printf("  -1 accepted: FAIL\n");
  }
  if(setpriority(50) < 0) {
    printf("  50 rejected: PASS\n");
  } else {
    printf("  50 accepted: FAIL\n");
  }
  if(setpriority(49) >= 0) {
    printf("  49 accepted: PASS\n");
  } else {
    printf("  49 rejected: FAIL\n");
  }
  if(setpriority(0) >= 0) {
    printf("  0 accepted: PASS\n");
  } else {
    printf("  0 rejected: FAIL\n");
  }
  printf("\n");

  // test inheritance
  printf("Test 4: Inheritance\n");
  setpriority(20);
  parent_priority = getpriority();
  printf("  Parent: %d\n", parent_priority);
  
  pid = fork();
  if(pid < 0) {
    printf("  fork failed\n");
    exit(1);
  }
  
  if(pid == 0) {
    child_priority = getpriority();
    printf("  Child: %d\n", child_priority);
    if(child_priority == parent_priority) {
      printf("  PASS\n");
    } else {
      printf("  FAIL\n");
    }
    exit(0);
  } else {
    wait(0);
  }
  printf("\n");

  // test multiple changes
  printf("Test 5: Multiple changes\n");
  int test_priorities[] = {5, 15, 25, 35, 45};
  for(int i = 0; i < 5; i++) {
    setpriority(test_priorities[i]);
    int current = getpriority();
    printf("  %d -> %d: %s\n", 
           test_priorities[i], current,
           (current == test_priorities[i]) ? "PASS" : "FAIL");
  }
  printf("\n");

  printf("Task 1 complete. Run 'ps' to check output format.\n\n");

  exit(0);
}

