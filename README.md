# Lab: file system

## Large files

inode content改成多级索引，以支持大文件

从12+256=268个块升级到支持256*256+256+11=65803个块

修改bmap，按需分配间接索引，无非就是再多读一次磁盘得到double indirect index，同时确保itrunc释放文件的所有block，注意需要调用brelse释放索引区块占用的buffer cache，不然后面usertests的writebig会panic，buffer cache不够

## Symbolic links

先在user/usys.pl和user/user.h添加条目，然后在kernel/sysfile.c里实现sys_symlink，同时还需要在kernel/syscall.c和kernel/syscall.h里添加symlink的系统调用号和系统调用表项

kernel/stat.h添加T_SYMLINK文件类型

添加O_NOFOLLOW标志位到kernel/fcntl.h，open系统调用会用到，带上这个标志位表示不跟踪，直接打开符号链接的文件

open的改动还是比较简单的，在open系统调用判断文件是不是符号链接，是的话就跟踪符号链接，要设置一个阈值防止循环引用

symlink的实现也很简单，调用create创建T_SYMLINK类型的文件即可，然后调用writei把target路径写入inode content

注意每个fs系统调用都需要包裹在begin_op()和end_op()里，通过事务保证fs一致性