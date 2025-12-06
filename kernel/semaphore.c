//
// Semaphore implementation
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

struct semtab semtable;

// Initialize the semaphore table
void
seminit(void)
{
  struct semaphore *s;
  
  initlock(&semtable.lock, "semtable");
  for(s = semtable.sem; s < semtable.sem + NSEM; s++){
    initlock(&s->lock, "semaphore");
    s->count = 0;
    s->allocated = 0;
  }
}

// Allocate a semaphore from the table
// Returns a pointer to the semaphore, or 0 if none available
struct semaphore*
semalloc(void)
{
  struct semaphore *s;
  
  acquire(&semtable.lock);
  for(s = semtable.sem; s < semtable.sem + NSEM; s++){
    if(s->allocated == 0){
      s->allocated = 1;
      s->count = 0;
      release(&semtable.lock);
      return s;
    }
  }
  release(&semtable.lock);
  return 0;
}

// Deallocate a semaphore
void
semdealloc(struct semaphore *s)
{
  if(s == 0)
    return;
  
  acquire(&semtable.lock);
  if(s < semtable.sem || s >= semtable.sem + NSEM){
    release(&semtable.lock);
    panic("semdealloc: invalid semaphore");
  }
  if(s->allocated == 0){
    release(&semtable.lock);
    panic("semdealloc: semaphore not allocated");
  }
  s->allocated = 0;
  s->count = 0;
  release(&semtable.lock);
}

