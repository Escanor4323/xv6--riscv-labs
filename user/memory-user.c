#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Memory user test program for demonstrating lazy allocation
// Usage: memory-user <iterations> <size_mb> <sleep_seconds>
int
main(int argc, char *argv[])
{
  if (argc != 4) {
    printf("Usage: memory-user <iterations> <size_mb> <sleep_seconds>\n");
    exit(1);
  }

  int iterations = atoi(argv[1]);
  int size_mb = atoi(argv[2]);
  int sleep_sec = atoi(argv[3]);

  printf("memory-user: iterations=%d size_mb=%d sleep_sec=%d\n",
         iterations, size_mb, sleep_sec);

  for (int i = 0; i < iterations; i++) {
    printf("Iteration %d: allocating %d MB\n", i + 1, size_mb);
    
    char *mem = sbrk(size_mb * 1024 * 1024);
    if (mem == (char*)-1) {
      printf("sbrk failed\n");
      exit(1);
    }
    
    // CASE 1: Do nothing - just allocate virtual memory
    // (No touching, so lazy allocation won't allocate physical pages)
    
    // CASE 2: Touch every page (uncomment to test)
    // for (int j = 0; j < size_mb * 1024 * 1024; j += 4096) {
    //   mem[j] = 1;
    // }
    
    // CASE 3: Touch ~1/16 of pages (uncomment to test)
    // for (int j = 0; j < size_mb * 1024 * 1024; j += 4096 * 16) {
    //   mem[j] = 1;
    // }
    
    printf("Sleeping for %d seconds...\n", sleep_sec);
    sleep(sleep_sec * 100);  // sleep takes ticks (100 ticks/sec)
    
    printf("Freeing memory...\n");
    if (sbrk(-size_mb * 1024 * 1024) == (char*)-1) {
      printf("sbrk (shrink) failed\n");
      exit(1);
    }
    
    printf("Sleeping for %d seconds after free...\n", sleep_sec);
    sleep(sleep_sec * 100);
  }
  
  printf("memory-user: done\n");
  exit(0);
}

