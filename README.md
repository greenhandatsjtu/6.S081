# Lab Multithreading

## Uthread: switching between threads

+ 实现用户态多线程的上下文切换
+ 测试线程循环打印一行并交出CPU
+ 要求修改`user/uthread.c`，完善`thread_create()` and `thread_schedule()`函数，以及`user/uthread_switch.S`，完善`thread_switch`
+ 要求线程在各自的栈执行`thread_create()`传入的函数
+ `thread_schedule()`需要调用`thread_switch`切换线程
+ `thread_switch`保存和恢复寄存器（callee-saved），存在`struct thread`中

### 实现

认真读过xv6 book多线程一章后还是有很多收获的，了解了上下文切换和线程的原理就比较好做了。首先给`struct thread`添加一个`context`，和xv6的`struct thread`存相同的寄存器，然后`thread_schedule`函数添加线程切换的函数，传入两个线程的context：

```c
thread_switch((uint64) &t->context, (uint64) &next_thread->context);
```

最后是`thread_create()`函数里面，初始化线程，设置`ra`（返回地址）为传入的函数的地址，设置`sp`（栈指针）为线程栈的结尾：

```c
  t->context.sp = (uint64) (t->stack + STACK_SIZE);
  t->context.ra = (uint64) func;
```

至于`uthread_switch.S`里的`thread_switch`，直接把xv6的`swtch`汇编代码搬过来即可。