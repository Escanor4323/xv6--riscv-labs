#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

extern struct semtab semtable;

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;
  struct proc *p = myproc();

  if(argint(0, &n) < 0)
    return -1;

  addr = p->sz;
  if (n == 0)
    return addr;

  uint64 new_sz = addr + n;
  if(new_sz < p->sz){
    return (uint64)-1;
  }
  p->sz = new_sz;
  /*old eager allocatoin, we don't call growproc right away for lazy allocatoin*/
  /*if(growproc(n) < 0)
    return -1;*/
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_freepmem(void)
{
  uint64 pages = kfreepages_count();
  return pages * PGSIZE;
}

uint64
sys_sem_init(void)
{
  uint64 sem_addr;
  int pshared;
  unsigned int value;
  struct semaphore *s;
  struct proc *p = myproc();
  int index;

  // Get arguments: sem_t* sem, int pshared, unsigned int value
  if(argaddr(0, &sem_addr) < 0)
    return -1;
  if(argint(1, &pshared) < 0)
    return -1;
  if(argint(2, (int*)&value) < 0)
    return -1;

  // Allocate a semaphore from the table
  s = semalloc();
  if(s == 0)
    return -1;

  // Calculate index: s - semtable.sem
  index = s - semtable.sem;
  
  // Initialize the semaphore count
  acquire(&s->lock);
  s->count = value;
  release(&s->lock);

  // Write the index back to user space
  if(copyout(p->pagetable, sem_addr, (char*)&index, sizeof(index)) < 0) {
    semdealloc(s);
    return -1;
  }

  return 0;
}

uint64
sys_sem_destroy(void)
{
  uint64 sem_addr;
  int index;
  struct semaphore *s;
  struct proc *p = myproc();

  // Get argument: sem_t* sem
  if(argaddr(0, &sem_addr) < 0)
    return -1;

  // Read the semaphore index from user space
  if(copyin(p->pagetable, (char*)&index, sem_addr, sizeof(index)) < 0)
    return -1;

  // Validate index
  if(index < 0 || index >= NSEM)
    return -1;

  s = &semtable.sem[index];

  // Deallocate the semaphore
  semdealloc(s);

  return 0;
}

uint64
sys_sem_wait(void)
{
  uint64 sem_addr;
  int index;
  struct semaphore *s;
  struct proc *p = myproc();

  // Get argument: sem_t* sem
  if(argaddr(0, &sem_addr) < 0)
    return -1;

  // Read the semaphore index from user space
  if(copyin(p->pagetable, (char*)&index, sem_addr, sizeof(index)) < 0)
    return -1;

  // Validate index
  if(index < 0 || index >= NSEM)
    return -1;

  s = &semtable.sem[index];

  // Check if semaphore is allocated
  acquire(&s->lock);
  if(s->allocated == 0) {
    release(&s->lock);
    return -1;
  }

  // Decrement count (P operation)
  s->count--;
  
  // If count is negative, sleep until woken
  while(s->count < 0) {
    sleep(s, &s->lock);
  }
  
  release(&s->lock);
  return 0;
}

uint64
sys_sem_post(void)
{
  uint64 sem_addr;
  int index;
  struct semaphore *s;
  struct proc *p = myproc();

  // Get argument: sem_t* sem
  if(argaddr(0, &sem_addr) < 0)
    return -1;

  // Read the semaphore index from user space
  if(copyin(p->pagetable, (char*)&index, sem_addr, sizeof(index)) < 0)
    return -1;

  // Validate index
  if(index < 0 || index >= NSEM)
    return -1;

  s = &semtable.sem[index];

  // Check if semaphore is allocated
  acquire(&s->lock);
  if(s->allocated == 0) {
    release(&s->lock);
    return -1;
  }

  // Increment count (V operation)
  s->count++;
  
  // Wake up any processes waiting on this semaphore
  wakeup(s);
  
  release(&s->lock);
  return 0;
}