// CS4375 HW3 – Priority Scheduler
// Joel Martínez Alvarado

#include "kernel/param.h"

struct pstat {
  int pid;
  enum procstate state;
  uint64 size;
  int ppid;
  char name[16];
  int priority;
  int age;
  int effective_priority;
};