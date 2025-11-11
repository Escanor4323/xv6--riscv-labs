#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  uint64 free_bytes = freepmem();
  
  // Check for -m flag (display in MB)
  if (argc == 2 && strcmp(argv[1], "-m") == 0) {
    // Display in megabytes
    uint64 free_mb = free_bytes / (1024 * 1024);
    printf("%d\n", (int)free_mb);
  } else {
    // Display in bytes
    printf("%d\n", (int)free_bytes);
  }
  
  exit(0);
}

