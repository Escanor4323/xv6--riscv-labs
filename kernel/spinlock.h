#include "param.h"

// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

// Semaphore structure
struct semaphore {
  struct spinlock lock;  // Lock to protect semaphore
  int count;             // Semaphore count value
  int allocated;         // Whether this semaphore is allocated
};

// Semaphore table
struct semtab {
  struct spinlock lock;  // Lock to protect semaphore table
  struct semaphore sem[NSEM];  // Array of semaphores
};

