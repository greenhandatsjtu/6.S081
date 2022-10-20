// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct freelist_per_cpu {
	struct spinlock lock;
	struct run *freelist;
};

struct {
  struct freelist_per_cpu freelists[NCPU];
} kmem;

void
kinit()
{
  for (int i=0; i< NCPU; i++) {
	  initlock(&kmem.freelists[i].lock, "kmem");
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  // get CPU ID
  push_off();
  int id = cpuid();
  pop_off();

  acquire(&kmem.freelists[id].lock);
  r->next = kmem.freelists[id].freelist;
  kmem.freelists[id].freelist = r;
  release(&kmem.freelists[id].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  // get CPU ID
  push_off();
  int id = cpuid();
  pop_off();

  acquire(&kmem.freelists[id].lock);
  r = kmem.freelists[id].freelist;
  if(r)
    kmem.freelists[id].freelist = r->next;
  release(&kmem.freelists[id].lock);

  if(!r) {
	  for (int i=0; i< NCPU; i++) {
		  if (i==id)
			  continue;
		  acquire(&kmem.freelists[i].lock);
		  r = kmem.freelists[i].freelist;
		  if(r)
			  kmem.freelists[i].freelist = r->next;
		  release(&kmem.freelists[i].lock);
		  if(r)
			  break;
	  }
  }

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
