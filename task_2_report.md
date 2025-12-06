# Task 2 Report: Semaphore Data Structures and Allocation

## Overview
This task involved creating the fundamental data structures for semaphore management in the xv6 kernel, including the semaphore structure, semaphore table, and the initialization and allocation functions.

## Changes Made

### 1. Data Structure Definitions (`kernel/spinlock.h`)
Added two new structures for semaphore management:

**struct semaphore**:
```c
struct semaphore {
  struct spinlock lock;  // Lock to protect semaphore
  int count;             // Semaphore count value
  int allocated;         // Whether this semaphore is allocated
};
```

**struct semtab**:
```c
struct semtab {
  struct spinlock lock;  // Lock to protect semaphore table
  struct semaphore sem[NSEM];  // Array of semaphores
};
```

**Location**: Lines 11-21 in `kernel/spinlock.h`

**Note**: Added `#include "param.h"` at the top of the file to ensure `NSEM` is defined when the header is included.

### 2. Semaphore Limit Constant (`kernel/param.h`)
Added the maximum number of semaphores constant:
```c
#define NSEM         100   // maximum number of semaphores
```

**Location**: Line 15 in `kernel/param.h`

This follows the same pattern as other system limits like `NPROC`, `NFILE`, etc.

### 3. Semaphore Implementation (`kernel/semaphore.c`)
Created a new file implementing the core semaphore management functions:

#### **seminit()**
- Initializes the semaphore table
- Initializes the table lock
- Iterates through all semaphores and:
  - Initializes each semaphore's lock
  - Sets count to 0
  - Marks as unallocated (allocated = 0)

**Location**: Lines 12-22 in `kernel/semaphore.c`

#### **semalloc()**
- Allocates a semaphore from the table
- Acquires the table lock
- Searches for the first unallocated semaphore
- Marks it as allocated and resets count to 0
- Returns a pointer to the semaphore, or 0 if none available

**Location**: Lines 24-38 in `kernel/semaphore.c`

#### **semdealloc()**
- Deallocates a semaphore
- Validates the semaphore pointer is within the table bounds
- Checks that the semaphore is actually allocated
- Marks it as unallocated and resets count to 0
- Includes panic calls for invalid operations

**Location**: Lines 40-55 in `kernel/semaphore.c`

**Global Variable**:
```c
struct semtab semtable;
```
This is the global semaphore table instance.

**Location**: Line 9 in `kernel/semaphore.c`

### 4. Function Declarations (`kernel/defs.h`)
Added declarations for the semaphore functions:
```c
// semaphore.c
void            seminit(void);
struct semaphore* semalloc(void);
void            semdealloc(struct semaphore*);
```

**Location**: Lines 197-200 in `kernel/defs.h`

### 5. Makefile Updates (`Makefile`)
Added `semaphore.o` to the kernel object files list:
```makefile
$K/semaphore.o
```

**Location**: Line 32 in `Makefile`

### 6. Kernel Initialization (`kernel/main.c`)
Added call to `seminit()` during kernel boot:
```c
seminit();       // semaphore table
```

**Location**: Line 30 in `kernel/main.c`

Placed after `fileinit()` and before `virtio_disk_init()`, following the initialization order pattern of other subsystems.

## Design Decisions

### Semaphore Structure
- **spinlock**: Each semaphore has its own lock to protect concurrent access to the semaphore's count and state
- **count**: The semaphore value (number of available resources)
- **allocated**: Boolean flag to track whether this semaphore slot is in use

### Semaphore Table
- **Global table**: Single global `semtable` instance manages all semaphores
- **Table lock**: Protects the allocation/deallocation operations
- **Fixed-size array**: Uses a fixed array of size `NSEM` (100) for simplicity and performance

### Allocation Strategy
- **First-fit allocation**: `semalloc()` searches for the first available semaphore
- **Linear search**: Simple O(n) search through the array
- **Bounds checking**: `semdealloc()` validates pointer is within table bounds

### Error Handling
- **Null pointer check**: `semdealloc()` checks for null pointer
- **Bounds validation**: Ensures semaphore pointer is within the table array
- **State validation**: Verifies semaphore is actually allocated before deallocation
- **Panic on errors**: Invalid operations cause kernel panic (appropriate for kernel code)

## Build Verification

### Compilation
- ✅ Kernel compiled successfully
- ✅ User programs compiled successfully
- ✅ Filesystem image created successfully

### Build Commands Executed
```bash
make clean
make
make fs.img
```

All builds completed without errors. The semaphore infrastructure is ready for use.

## Integration Points

### Initialization Order
The `seminit()` call is placed in the kernel boot sequence after:
- Process table initialization (`procinit()`)
- File table initialization (`fileinit()`)

And before:
- Disk initialization (`virtio_disk_init()`)
- First user process (`userinit()`)

This ensures semaphores are available early in the boot process.

### Header Dependencies
- `spinlock.h` now includes `param.h` to ensure `NSEM` is defined
- This makes the header self-contained for user programs that include it

## Next Steps

The semaphore data structures and allocation functions are now in place. The next tasks will involve:
- Implementing the actual semaphore operations (`sem_wait` and `sem_post`) in the syscall handlers
- Adding wait queue support for blocking operations
- Testing the semaphore implementation with the producer-consumer program

## Notes

- The current implementation provides the foundation for semaphore operations
- Semaphores are allocated from a fixed pool of 100 semaphores
- Each semaphore has its own lock for thread-safe operations
- The allocation/deallocation functions are ready to be called from the syscall handlers
- The `count` field will be set by `sem_init` syscall and modified by `sem_wait` and `sem_post`

