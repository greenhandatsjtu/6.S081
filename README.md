# 6.S081
6.S081 2021 Fall Labs

## Schedule

+ [x] Setup 2021/11/18
+ [ ] Utilities
+ [ ] System calls
+ [ ] Page tables
+ [ ] Traps
+ [ ] Copy-on-write
+ [ ] Multithreading
+ [ ] Network driver
+ [ ] Lock
+ [ ] File system
+ [ ] Mmap

## Setup

Build Docker image using Dockerfile port from [CalvinHaynes/MIT6.S081-2020Fall](https://github.com/CalvinHaynes/MIT6.S081-2020Fall/blob/main/DockerFIle/Dockerfile)
```shell
docker build -t nil/6s081 .
```
Run container and mount xv6 root:
```shell
docker run -itd --name xv6 -v $(pwd):/xv6 nil/6s081
```
To test your installation, compile and run xv6 using `docker exec`:
```shell
docker exec -it xv6 make qemu
```

Output:
```shell
# ... lots of output ...
xv6 kernel is booting

hart 1 starting
hart 2 starting
init: starting sh
$
```

You may also check QEMU and gcc version:
```shell
$ docker exec -it xv6 qemu-system-riscv64 --version

QEMU emulator version 5.1.0
Copyright (c) 2003-2020 Fabrice Bellard and the QEMU Project developers

$ docker exec -it xv6 riscv64-linux-gnu-gcc --version

riscv64-linux-gnu-gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```