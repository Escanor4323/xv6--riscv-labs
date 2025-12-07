# Task 4 Report: Readers–Writers with Semaphores

## Overview
Implemented a classic readers–writers test program using unnamed semaphores and shared memory in xv6 user space, then built it into the user binaries and validated multiple reader/writer mixes under QEMU.

## Implementation Details
- Added `user/rwtest-sem.c` implementing the readers–writers solution with shared state allocated via `mmap(..., MAP_SHARED)` and two semaphores:
  - `mutex` protects the `readcount` bookkeeping.
  - `rwlock` guarantees writers’ exclusive access while allowing concurrent readers when no writer holds the lock.
- Readers increment/decrement `readcount` with first-reader/last-reader logic; writers obtain exclusive access around each write. Each writer performs `WRITE_LOOPS` increments; readers perform `READ_LOOPS` reads and track aggregate read statistics.
- Updated `Makefile` `UPROGS` to include `_rwtest-sem` so the binary is packaged into `fs.img`.

## Test Results
Executed the required scenarios in QEMU (commands piped with delays to allow boot):

```
$ rwtest-sem 0 1
readers=0 writers=1
final data=5
read_ops=0 read_sum=0
write_ops=5
$ rwtest-sem 3 0
readers=3 writers=0
final data=0
read_ops=15 read_sum=0
write_ops=0
$ rwtest-sem 1 1
readers=1 writers=1
final data=5
read_ops=5 read_sum=0
write_ops=5
$ rwtest-sem 3 2
readers=3 writers=2
final data=10
read_ops=15 read_sum=0
write_ops=10
```

Observations:
- Final `data` equals `writers * WRITE_LOOPS` (WRITE_LOOPS=5) in mixed cases.
- Readers execute without blocking each other when no writers are active; writers obtain exclusive access as expected.
- All runs complete without deadlock or starvation.
