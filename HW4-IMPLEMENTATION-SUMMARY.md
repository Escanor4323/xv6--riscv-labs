# HW4: Lazy Allocation Implementation Summary

## Overview
Successfully implemented lazy memory allocation for xv6-riscv. All tasks completed as specified in the assignment.

## Branch
```bash
git checkout hw4-lazy-alloc
```

## Implementation Details

### Task 1: `freepmem()` Syscall ✅

**What was implemented:**
- Added `freepmem()` syscall to query available physical memory
- Implemented O(1) free memory tracking in `kalloc.c` using a `free_bytes` counter
- Created `user/free.c` command that displays free memory in bytes or MB (`-m` flag)
- Created `user/memory-user.c` test program for demonstrating lazy allocation

**Files modified:**
- `kernel/kalloc.c`: Added `free_bytes` counter and `kfreebytes()` function
- `kernel/defs.h`: Added `kfreebytes()` declaration
- `kernel/syscall.h`: Added `SYS_freepmem` (25)
- `kernel/syscall.c`: Added syscall dispatch
- `kernel/sysproc.c`: Implemented `sys_freepmem()`
- `user/user.h`: Added `freepmem()` declaration
- `user/usys.pl`: Added `freepmem` entry
- `user/free.c`: Created free memory display command
- `user/memory-user.c`: Created test program
- `Makefile`: Added `_free` and `_memory-user` to UPROGS

**Key implementation:**
```c
// kalloc.c - O(1) tracking
struct {
  struct spinlock lock;
  struct run *freelist;
  uint64 free_bytes;  // Track free physical memory
} kmem;

uint64 kfreebytes(void) {
  uint64 bytes;
  acquire(&kmem.lock);
  bytes = kmem.free_bytes;
  release(&kmem.lock);
  return bytes;
}
```

---

### Task 2: Virtual-Only `sbrk()` ✅

**What was implemented:**
- Modified `sys_sbrk()` to only adjust `p->sz` without allocating physical pages
- Growing heap: increment `p->sz` with overflow and MAXVA checks
- Shrinking heap: call `uvmdealloc()` to unmap pages
- Physical pages allocated on-demand in Task 3

**Files modified:**
- `kernel/sysproc.c`: Completely rewrote `sys_sbrk()`

**Key implementation:**
```c
uint64 sys_sbrk(void) {
  int n;
  uint64 addr;
  struct proc *p = myproc();

  if(argint(0, &n) < 0)
    return -1;
  
  addr = p->sz;
  
  if(n > 0) {
    // Growing: just increase p->sz (lazy allocation)
    if(p->sz + n < p->sz || p->sz + n > MAXVA)
      return -1;
    p->sz += n;
  } else if(n < 0) {
    // Shrinking: unmap pages
    p->sz = uvmdealloc(p->pagetable, p->sz, p->sz + n);
  }
  
  return addr;
}
```

---

### Task 3: Page Fault Handler (Alloc-on-Fault) ✅

**What was implemented:**
- Added page fault detection in `usertrap()` for load (scause 13) and store (scause 15) faults
- Validate fault address is in valid heap range: `PGSIZE <= va < p->sz < TRAPFRAME`
- Allocate physical page using `kalloc()` on first access
- Zero allocated pages for security
- Map page with `PTE_U | PTE_R | PTE_W` (no execute for data)
- Print clear message when out of physical memory
- Kill process on invalid addresses

**Files modified:**
- `kernel/trap.c`: Added page fault handling in `usertrap()`

**Key implementation:**
```c
// In usertrap()
else {
  uint64 scause = r_scause();
  
  if(scause == 13 || scause == 15) {  // Load or store page fault
    uint64 fault_va = r_stval();
    uint64 va = PGROUNDDOWN(fault_va);
    
    if(va >= PGSIZE && va < p->sz && va < TRAPFRAME) {
      char *pa = kalloc();
      if(pa == 0) {
        printf("usertrap(): out of physical memory for lazy allocation pid=%d\n", p->pid);
        p->killed = 1;
      } else {
        memset(pa, 0, PGSIZE);
        if(mappages(p->pagetable, va, PGSIZE, (uint64)pa, PTE_U | PTE_R | PTE_W) != 0) {
          kfree(pa);
          p->killed = 1;
        }
      }
    } else {
      printf("usertrap(): invalid page fault address %p pid=%d\n", fault_va, p->pid);
      p->killed = 1;
    }
  }
}
```

---

### Task 4: Tolerate Unmapped Pages ✅

**What was implemented:**
- Modified `uvmunmap()` to gracefully skip unmapped pages instead of panicking
- Modified `uvmcopy()` to handle unmapped pages during fork
- Parent's mapped pages are copied to child
- Parent's unmapped pages remain unmapped in child (lazy allocation preserved)

**Files modified:**
- `kernel/vm.c`: Modified `uvmunmap()` and `uvmcopy()`

**Key implementation:**
```c
// uvmunmap() - tolerate unmapped pages
for(a = va; a < va + npages*PGSIZE; a += PGSIZE){
  if((pte = walk(pagetable, a, 0)) == 0)
    continue;  // Skip missing page table entries
  if((*pte & PTE_V) == 0)
    continue;  // Skip unmapped pages
  // ... rest of unmapping logic
}

// uvmcopy() - handle unmapped parent pages
for(i = 0; i < sz; i += PGSIZE){
  if((pte = walk(old, i, 0)) == 0)
    continue;  // Parent page not mapped - leave child unmapped
  if((*pte & PTE_V) == 0)
    continue;  // Parent page not present - leave child unmapped
  // ... copy mapped pages
}
```

---

## Testing Instructions

### Quick Test (Case 1 - Virtual-Only Allocation)
```bash
make qemu
# In xv6 shell:
free
memory-user 1 10 5 &
free
# Wait 5 seconds
free
```

**Expected**: Free memory should NOT decrease after `sbrk()` because pages are not touched.

### Full Test Suite
See `HW4-TESTS.md` for complete testing procedures for all three cases:
1. **Case 1**: Allocate without touching (pure lazy allocation)
2. **Case 2**: Touch every page (full physical allocation)
3. **Case 3**: Touch ~1/16 of pages (sparse allocation)

To run different cases, edit `user/memory-user.c` and uncomment the appropriate CASE section, then rebuild with `make`.

---

## Commits

All changes committed with clear commit messages:

1. `feat(syscall): add freepmem() syscall and user commands`
2. `feat(vm): make sbrk() virtual-only for lazy allocation`
3. `feat(trap): handle user page faults with alloc-on-fault`
4. `fix(vm): tolerate unmapped pages in uvmunmap/uvmcopy`

---

## Verification Checklist

All requirements met:

- ✅ `freepmem()` syscall implemented with O(1) tracking
- ✅ `free` command prints bytes
- ✅ `free -m` command prints MB
- ✅ `memory-user` test program created
- ✅ `sbrk()` is virtual-only (no `growproc()` call)
- ✅ Page faults (scause 13, 15) trigger lazy allocation
- ✅ Fault addresses validated (PGSIZE to p->sz, below TRAPFRAME)
- ✅ Pages allocated with `kalloc()` and zeroed
- ✅ Pages mapped with user read/write permissions (no execute)
- ✅ Clear "out of memory" message on `kalloc()` failure
- ✅ `uvmunmap()` tolerates unmapped pages
- ✅ `uvmcopy()` handles unmapped pages during fork
- ✅ No panics on unmapped pages
- ✅ Overflow and MAXVA bounds checking in `sbrk()`

---

## Design Decisions

### Free Memory Tracking
Used a simple counter instead of walking the freelist for O(1) performance. Counter is protected by `kmem.lock` and updated atomically.

### Page Fault Validation
Strict validation prevents:
- Null pointer dereferences (va < PGSIZE)
- Access beyond heap (va >= p->sz)
- Access to trampoline/trapframe regions (va >= TRAPFRAME)

### Fork Behavior
Preserves lazy allocation semantics: if parent hasn't touched a page, child won't have it either. Child will fault and allocate independently.

### Unmapped Page Handling
`uvmunmap()` and `uvmcopy()` use `continue` instead of `panic()` when encountering unmapped pages. This is safe because:
- Missing PTEs mean no physical memory to free
- Children don't need copies of pages that were never allocated

### Heap Shrinking
Still uses `uvmdealloc()` to properly unmap and free physical pages. The modified `uvmunmap()` handles the case where some pages in the range were never allocated.

---

## Known Limitations

### `copyout`/`copyin` Behavior
These kernel functions will fail if trying to access unmapped user pages. This is acceptable because:
- It's reasonable to require users to touch memory before passing to syscalls
- Prevents kernel from triggering page faults
- Matches assignment guidance ("figure out causes" not "fix everything")

### Single Address Space
Current implementation doesn't handle multiple address spaces or huge pages, per standard xv6 limitations.

---

## Build and Run

```bash
# Clean build
make clean && make

# Run in QEMU
make qemu

# Run tests (inside xv6)
free
free -m
memory-user 1 10 5 &
```

---

## Conclusion

Successfully implemented lazy memory allocation with all required features:
1. Virtual-only heap growth
2. On-demand physical page allocation
3. Proper handling of unmapped pages
4. Free memory tracking and reporting

The implementation follows xv6 coding conventions, includes clear comments, and handles edge cases gracefully.

