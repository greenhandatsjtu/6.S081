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


uint page_refcount[(PHYSTOP-KERNBASE)/PGSIZE]; // track all physical pages' reference count

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
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

  if(page_refcount[PA_INDEX(pa)] > 1){
	  page_refcount[PA_INDEX(pa)]--;
	  return;
  }
  page_refcount[PA_INDEX(pa)] = 0;

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r) {
	  memset((char *) r, 5, PGSIZE); // fill with junk
	  page_refcount[PA_INDEX(r)] = 1;

  }
  return (void*)r;
}

int cow_alloc(pagetable_t pagetable, uint64 va){
	if(va >= MAXVA) return -1;
	char *mem;
	pte_t *pte = walk(pagetable, va, 0);
	if(pte==0) return -1;
	uint64 pa = PTE2PA(*pte);
	if(pa == 0 || pa < (uint64)end) return -1;
	uint flags = PTE_FLAGS(*pte);
	if((mem = kalloc())==0) return -1;
	// copy page
	memmove(mem, (char*)pa, PGSIZE);
	// unmap page
	// must pass 1 as third param (do_free), which calls kfree() and decrements refcount of physical page
	uvmunmap(pagetable, va, 1, 1);
	// map new page with PTE_W set
	if (mappages(pagetable, va, PGSIZE, (uint64)mem, (flags|PTE_W)&~PTE_C) != 0) {
		kfree((void *)mem);
		return -1;
	}
	return 0;
}
