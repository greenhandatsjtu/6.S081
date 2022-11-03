// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKET 13
#define BUCSZ 10

struct bucket {
	struct spinlock lock;
	struct buf bufs[BUCSZ];
};

struct {
	struct spinlock lock;

	struct bucket bucs[NBUCKET];
} bcache;


void
binit(void) {
	struct bucket *buc;

	initlock(&bcache.lock, "bcache");
	for (int i = 0; i < NBUCKET; i++) {
		buc = &bcache.bucs[i];
		initlock(&buc->lock, "bcache");
		for (int j = 0; j < BUCSZ; j++) {
			initsleeplock(&buc->bufs[j].lock, "buffer");
		}
	}
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf *
bget(uint dev, uint blockno) {
	struct buf *b;
	struct bucket *buc;

	int ind = blockno % NBUCKET; // calculate hash value
	buc = &bcache.bucs[ind];
	acquire(&buc->lock);

	// if the block already cached
	for (int i = 0; i < BUCSZ; i++) {
		b = &buc->bufs[i];
		if (b->dev == dev && b->blockno == blockno) {
			b->refcnt++;
			release(&buc->lock);
			acquiresleep(&b->lock);
			return b;
		}
	}

	// Not cached
	uint min_ticks = ticks + 1;
	struct buf *lru_b = 0;
	for (int i = 0; i < BUCSZ; i++) {
		b = &buc->bufs[i];
		if (b->ts < min_ticks && b->refcnt == 0) {
			min_ticks = b->ts;
			lru_b = b;
		}
	}
	if (lru_b != 0) {
		lru_b->dev = dev;
		lru_b->blockno = blockno;
		lru_b->valid = 0;
		lru_b->refcnt = 1;
		release(&buc->lock);
		acquiresleep(&lru_b->lock);
		return lru_b;
	}

	panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf *
bread(uint dev, uint blockno) {
	struct buf *b;

	b = bget(dev, blockno);
	if (!b->valid) {
		virtio_disk_rw(b, 0);
		b->valid = 1;
	}
	return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b) {
	if (!holdingsleep(&b->lock))
		panic("bwrite");
	virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b) {
	if (!holdingsleep(&b->lock))
		panic("brelse");

	releasesleep(&b->lock);
	struct bucket *buc;
	int ind = b->blockno % NBUCKET; // calculate hash value
	buc = &bcache.bucs[ind];
	acquire(&buc->lock);
	b->refcnt--;
	if (b->refcnt == 0) {
		// no one is waiting for it.
		b->ts = ticks;
	}
	release(&buc->lock);
}

void
bpin(struct buf *b) {
	uint key = b->blockno % NBUCKET;
	acquire(&bcache.bucs[key].lock);
	b->refcnt++;
	release(&bcache.bucs[key].lock);
}

void
bunpin(struct buf *b) {
	uint key = b->blockno % NBUCKET;
	acquire(&bcache.bucs[key].lock);
	b->refcnt--;
	if (b->refcnt == 0) {
		b->ts = ticks;
	}
	release(&bcache.bucs[key].lock);
}


