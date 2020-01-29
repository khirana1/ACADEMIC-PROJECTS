// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
uint freeFrames(void);
uint get_pgRef_cnt(uint pa);
void increment_pgRef_cnt(uint pa);
void decrement_pgRef_cnt(uint pa);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  uint pgRef_cnt[PHYSTOP >> PGSHIFT];
} kmem;

int free_frame_cnt = 0; // xv6 proj - cow

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  free_frame_cnt = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
  {
    kmem.pgRef_cnt[V2P(p) >> PGSHIFT] = 0; //Initialize ref count to 0
    kfree(p);
  }
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  //memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  if(kmem.pgRef_cnt[V2P(v) >> PGSHIFT] > 0)
     --kmem.pgRef_cnt[V2P(v) >> PGSHIFT];
  if(kmem.pgRef_cnt[V2P(v) >> PGSHIFT] == 0)
  {
    memset(v, 1, PGSIZE); 
    free_frame_cnt++; //Increment free pages no. when there is free page
    r->next = kmem.freelist;
    kmem.freelist = r;
    //free_frame_cnt++; // xv6 proj - cow
  }
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
  {
    kmem.freelist = r->next;
	free_frame_cnt--; // xv6 proj - cow. After allocation decrease no. of free pages
    kmem.pgRef_cnt[V2P((char*)r) >> PGSHIFT] = 1; //On allocation ref count is set to 1
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

uint freeFrames(void)
{
  acquire(&kmem.lock);
  uint free_frame_cnt = free_frame_cnt;
  release(&kmem.lock);
  return free_frame_cnt;
}

uint get_pgRef_cnt(uint pa)
{
  if(pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("Get Page Reference Count.");
  uint count;
  acquire(&kmem.lock);
  count = kmem.pgRef_cnt[pa >> PGSHIFT];
  release(&kmem.lock);
  return count;
}

void increment_pgRef_cnt(uint pa)
{
  if(pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("Increment Page Reference Count.");
  acquire(&kmem.lock);
  ++kmem.pgRef_cnt[pa >> PGSHIFT];
  release(&kmem.lock);
}

void decrement_pgRef_cnt(uint pa)
{
  if(pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("Decrement Page Reference Count.");
  acquire(&kmem.lock);
  --kmem.pgRef_cnt[pa >> PGSHIFT];
  release(&kmem.lock);
}
