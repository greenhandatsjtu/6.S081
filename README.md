# Lab locks

## Memory allocator

+ 三个进程不断收缩地址空间，于是会不断调用`kalloc`和`kfree`，它们均会获取`kmem.lock`锁
+ `acquire`会记录：
  + 每个锁被获取的次数
  + 循环尝试获取锁的次数
+ 最后打印`kmem`和`bcache`的锁的信息，以及5个竞争最多的锁的信息
+ 本次assignment中锁竞争的根源为，`kalloc`访问free链表，而该链表由锁保护，各个CPU均需要获取这个锁
+ 因此需要为每个CPU单独维护free链表
+ 如果出现一个CPU的free链表空了，而另一个CPU的free链表还有空闲内存，则需要从该CPU“窃取”

### 实现

将原有单链表实现改为链表数组，元素个数为`NCPU`，`kinit`、`kfree`和`kalloc`均修改为以`cpuid`为下标访问对应free链表即可。

```c
struct freelist_per_cpu {
	struct spinlock lock;
	struct run *freelist;
};

struct {
  struct freelist_per_cpu freelists[NCPU];
} kmem;
```

需要注意，如果当前CPU的free链表为空，需要遍历所有free链表，从还有空闲内存的CPU处窃取

```c
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
```

### 测评

![](images/kalloc.png)