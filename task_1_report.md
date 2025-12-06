# Task 1 Report: Syscall Declarations and prodcons-sem Registration

## Overview
This task involved adding syscall declarations for semaphore operations (`sem_init`, `sem_destroy`, `sem_wait`, `sem_post`), wiring them into the xv6 system call infrastructure, adding the `sem_t` type definition, and creating a producer-consumer program that uses semaphores.

## Changes Made

### 1. User Space Syscall Prototypes (`user/user.h`)
Added function prototypes for the four semaphore syscalls:
```c
int sem_init(sem_t*, int, unsigned int);
int sem_destroy(sem_t*);
int sem_wait(sem_t*);
int sem_post(sem_t*);
```

**Location**: Lines 27-30 in `user/user.h`

### 2. User Space Syscall Stubs (`user/usys.pl`)
Added entries to generate assembly stubs for the semaphore syscalls:
- `entry("sem_init");`
- `entry("sem_destroy");`
- `entry("sem_wait");`
- `entry("sem_post");`

**Location**: Lines 42-45 in `user/usys.pl`

### 3. Kernel Syscall Numbers (`kernel/syscall.h`)
Added system call number definitions:
```c
#define SYS_sem_init    25
#define SYS_sem_destroy 26
#define SYS_sem_wait    27
#define SYS_sem_post    28
```

**Location**: Lines 26-29 in `kernel/syscall.h`

### 4. Kernel Syscall Table (`kernel/syscall.c`)
Added external function declarations:
```c
extern uint64 sys_sem_init(void);
extern uint64 sys_sem_destroy(void);
extern uint64 sys_sem_wait(void);
extern uint64 sys_sem_post(void);
```

Added entries to the syscalls array:
```c
[SYS_sem_init]    sys_sem_init,
[SYS_sem_destroy] sys_sem_destroy,
[SYS_sem_wait]    sys_sem_wait,
[SYS_sem_post]    sys_sem_post,
```

**Location**: 
- External declarations: Lines 110-113 in `kernel/syscall.c`
- Array entries: Lines 137-140 in `kernel/syscall.c`

### 5. Semaphore Type Definition (`kernel/types.h`)
Added the `sem_t` type definition:
```c
typedef int sem_t;
```

**Location**: Line 12 in `kernel/types.h`

### 6. Stub Syscall Implementations (`kernel/sysproc.c`)
Added stub implementations for the semaphore syscalls. These are placeholder functions that return 0 (success) and will be fully implemented in later tasks:
```c
uint64 sys_sem_init(void)
uint64 sys_sem_destroy(void)
uint64 sys_sem_wait(void)
uint64 sys_sem_post(void)
```

**Location**: Lines 118-141 in `kernel/sysproc.c`

### 7. Producer-Consumer Program (`user/prodcons-sem.c`)
Created a new user program that demonstrates producer-consumer synchronization using semaphores:

**Features**:
- Uses `mmap` to create a shared buffer in memory
- Implements a circular buffer with size `BSIZE = 10`
- Producer process produces `MAX = 10` items
- Consumer process consumes all items
- Uses three semaphores:
  - `empty`: Tracks available buffer slots (initialized to BSIZE)
  - `full`: Tracks filled buffer slots (initialized to 0)
  - `mutex`: Protects critical sections (initialized to 1)
- Uses `fork()` to create separate producer and consumer processes
- Properly cleans up semaphores with `sem_destroy()`

**Location**: `user/prodcons-sem.c` (new file, 95 lines)

### 8. Makefile Updates (`Makefile`)
Added `_prodcons-sem` to the `UPROGS` list to include it in the filesystem image:
```makefile
$U/_prodcons-sem\
```

**Location**: Line 138 in `Makefile`

## Build Verification

### Compilation
- ✅ Kernel compiled successfully
- ✅ User program `prodcons-sem.c` compiled successfully
- ✅ Filesystem image (`fs.img`) created with `_prodcons-sem` included

### Build Commands Executed
```bash
make clean
make
make fs.img
```

All builds completed without errors. The system is ready for QEMU execution.

## Git Operations

### Commit
Successfully committed all changes with message:
```
Task 1: Added syscall declarations and prodcons-sem registration
```

### Files Changed
- `user/user.h` - Added syscall prototypes
- `user/usys.pl` - Added syscall stub entries
- `kernel/syscall.h` - Added syscall numbers
- `kernel/syscall.c` - Added syscall table entries
- `kernel/types.h` - Added `sem_t` typedef
- `kernel/sysproc.c` - Added stub implementations
- `user/prodcons-sem.c` - New producer-consumer program
- `Makefile` - Added program to UPROGS

**Total**: 8 files changed, 149 insertions(+), 5 deletions(-)

### Push Status
- ✅ Local commit successful
- ⚠️ Remote push requires authentication (manual push may be needed)

## System Call Flow

The syscall infrastructure is now wired as follows:

1. **User Program** calls `sem_init()`, `sem_wait()`, etc.
2. **usys.S** (generated from `usys.pl`) contains assembly stubs that:
   - Load the syscall number into register `a7`
   - Execute `ecall` instruction
   - Return result
3. **Kernel trap handler** routes to `syscall()` function
4. **syscall.c** looks up the syscall number in the `syscalls[]` array
5. **sysproc.c** contains the actual implementation (currently stubs)

## Next Steps

The syscall declarations and wiring are complete. The next tasks will involve:
- Implementing the actual semaphore data structures in the kernel
- Implementing the semaphore operations (`init`, `destroy`, `wait`, `post`)
- Testing the producer-consumer program with fully functional semaphores

## Notes

- The current syscall implementations are stubs that return 0 (success). They need to be fully implemented in subsequent tasks.
- The `prodcons-sem.c` program is ready to use once the semaphore syscalls are fully implemented.
- All syscall numbers follow the existing pattern and are sequentially assigned (25-28).
- The `sem_t` type is defined as `int` in the kernel, which is a common approach for semaphore handles.


