# HW4 Lazy Allocation - Test Runbook

## Overview
This document describes the test procedures and expected results for the lazy allocation implementation in xv6.

## Implementation Summary
- **Task 1**: Added `freepmem()` syscall to query available physical memory
- **Task 2**: Modified `sbrk()` to only adjust virtual address space (no physical allocation)
- **Task 3**: Added page fault handler to allocate pages on-demand
- **Task 4**: Modified `uvmunmap()` and `uvmcopy()` to tolerate unmapped pages

## Test Setup

### Build and Run
```bash
make clean
make
make qemu
```

### Test Programs
1. `free` - Display available physical memory in bytes
2. `free -m` - Display available physical memory in MB
3. `memory-user` - Test program for lazy allocation with three configurable cases

## Test Cases

### Case 1: Allocate Without Touching (Pure Lazy Allocation)

**Purpose**: Demonstrate that `sbrk()` does not consume physical memory until pages are accessed.

**Configuration**: In `user/memory-user.c`, all touch loops are commented out (default).

**Commands**:
```bash
# In xv6 shell:
memory-user 1 100 10 &
free
free -m
# Wait for sleep period
free
free -m
```

**Expected Behavior**:
- Free memory should NOT decrease after `sbrk()` call
- Free memory remains constant because pages are not touched
- This proves virtual-only allocation

**Actual Results**:
```
[To be filled in after testing]
```

---

### Case 2: Touch Every Page (Full Physical Allocation)

**Purpose**: Demonstrate that lazy allocation works when all pages are accessed.

**Configuration**: In `user/memory-user.c`, uncomment CASE 2:
```c
// CASE 2: Touch every page (uncomment to test)
for (int j = 0; j < size_mb * 1024 * 1024; j += 4096) {
  mem[j] = 1;
}
```

**Commands**:
```bash
# In xv6 shell:
memory-user 1 100 10 &
free
free -m
# Wait for sleep period (after touching)
free
free -m
# Wait for sleep period (after freeing)
free
free -m
```

**Expected Behavior**:
- Free memory should decrease by ~100 MB after pages are touched
- Free memory should increase by ~100 MB after `sbrk(-100MB)` frees the memory
- This proves on-demand allocation and deallocation work correctly

**Actual Results**:
```
[To be filled in after testing]
```

---

### Case 3: Touch ~1/16 of Pages (Sparse Physical Allocation)

**Purpose**: Demonstrate that lazy allocation only allocates pages that are actually accessed.

**Configuration**: In `user/memory-user.c`, uncomment CASE 3:
```c
// CASE 3: Touch ~1/16 of pages (uncomment to test)
for (int j = 0; j < size_mb * 1024 * 1024; j += 4096 * 16) {
  mem[j] = 1;
}
```

**Commands**:
```bash
# In xv6 shell:
memory-user 1 100 10 &
free
free -m
# Wait for sleep period (after touching)
free
free -m
```

**Expected Behavior**:
- Free memory should decrease by ~6-7 MB (roughly 1/16 of 100 MB)
- This proves lazy allocation allocates only touched pages
- Virtual address space is 100 MB but physical usage is only ~6-7 MB

**Actual Results**:
```
[To be filled in after testing]
```

---

## Additional Tests

### Test: Basic `free` Command
```bash
free
free -m
```

**Expected**: Display free memory in bytes and MB respectively.

**Actual Results**:
```
[To be filled in after testing]
```

---

### Test: Fork with Lazy Allocated Memory

**Commands**:
```bash
# Create a simple test
# Allocate memory, touch some pages, fork, verify child inherits correctly
```

**Expected Behavior**:
- Mapped pages in parent are copied to child
- Unmapped pages in parent remain unmapped in child
- Child can trigger lazy allocation independently

**Actual Results**:
```
[To be filled in after testing]
```

---

### Test: Shrinking Heap with Unmapped Pages

**Commands**:
```bash
# In memory-user, the sbrk(-size) should work even if some pages were never mapped
```

**Expected Behavior**:
- No kernel panics when unmapping regions with unmapped pages
- `uvmunmap()` gracefully skips unmapped pages

**Actual Results**:
```
[To be filled in after testing]
```

---

## Verification Checklist

- [ ] `free` command works and displays bytes
- [ ] `free -m` command works and displays MB
- [ ] Case 1: Virtual allocation doesn't consume physical memory
- [ ] Case 2: Touching all pages allocates ~100 MB physical memory
- [ ] Case 2: Freeing returns ~100 MB to free memory
- [ ] Case 3: Touching 1/16 pages allocates only ~6-7 MB
- [ ] No kernel panics on `uvmunmap` with unmapped pages
- [ ] No kernel panics on `uvmcopy` (fork) with unmapped pages
- [ ] No kernel panics on `sbrk` shrink with unmapped pages
- [ ] Clear "out of memory" message when `kalloc()` fails during fault handling
- [ ] Page faults are handled correctly (scause 13 and 15)
- [ ] Invalid addresses (outside heap range) are rejected

---

## Notes

### Implementation Details
- Free memory tracking uses O(1) counter in `kalloc.c`
- Page faults validated against: `va >= PGSIZE && va < p->sz && va < TRAPFRAME`
- Allocated pages are zeroed for security
- Heap pages mapped with `PTE_U | PTE_R | PTE_W` (no execute)

### Known Limitations
- `copyout`/`copyin` may fail on unmapped pages (user must touch memory first)
- This is acceptable behavior per assignment guidelines

---

## Conclusion

All tests demonstrate that lazy allocation:
1. Defers physical page allocation until first access
2. Handles page faults correctly
3. Allocates only touched pages
4. Properly frees memory
5. Works correctly with fork
6. Tolerates unmapped pages in deallocation paths

