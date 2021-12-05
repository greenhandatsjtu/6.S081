#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 va; // starting virtual address of the first page to check
  int n; // number of pages
  uint64 p; //address to user space buffer
  if(argaddr(0, &va) < 0)
	  return -1;
  if(argint(1, &n) < 0)
	  return -1;
  if(argaddr(2, &p) < 0)
	  return -1;

  // upper limit of the number of pages is 64
  if(n > 64)
	  return -1;

  pagetable_t pagetable = myproc()->pagetable;
//  vmprint(pagetable);
  uint64 abits=0; // access bitmask

  for(int i=0;i < n; i++){
	  pte_t *pte = walk(pagetable, va+i*PGSIZE, 0);
	  if(pte!=0 && (*pte&PTE_A)!=0){
		  abits |= 1<<i; // set access bit in access bitmask
		  *pte &= ~PTE_A; // clear PTE_A
	  }
  }

  // copy bitmask to user space
  return copyout(pagetable, p, (void *)&abits, sizeof(uint64));
}
#endif

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
