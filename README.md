# 6.S081
6.S081 2021 Fall Labs

## Schedule

Plan to finish one lab per week.

+ [x] ~~2021/11/18 **Setup**~~
+ [x] ~~2021/11/21 **Utilities**~~
+ [x] ~~2021/11/28 **System calls**~~
+ [x] ~~2021/12/05 **Page tables**~~
+ [x] ~~2021/12/12 **Traps**~~
+ [x] ~~2021/12/19 (2022/03/23) **Copy-on-write**~~
+ [x] ~~2021/12/26 (2022/04/05) **Multithreading**~~
+ [ ] 2022/01/02 **Network driver**
+ [ ] 2022/01/09 **Lock**
+ [ ] 2022/01/16 **File system**
+ [ ] 2022/01/23 **Mmap**

## Setup

Build Docker image using [Dockerfile](https://github.com/greenhandatsjtu/6.S081/blob/main/Dockerfile) port from [CalvinHaynes/MIT6.S081-2020Fall](https://github.com/CalvinHaynes/MIT6.S081-2020Fall/blob/main/DockerFIle/Dockerfile)
```shell
docker build -t nil/6s081 .
```
Then setup git and fetch xv6:
```shell
git remote add upstream git://g.csail.mit.edu/xv6-labs-2021
git fetch upstream

# prepare for lab 1 utilities
git checkout -b util upstream/util
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

### Test
Run all tests:
```shell
docker exec -it xv6 make grade
```

Run the grade tests that match "sleep"
```shell
docker exec -it xv6 make GRADEFLAGS=sleep grade
```

### Debug
First start gdb server:
```shell
docker exec -it xv6 make qemu-gdb
```

Then use `gdb-multiarch` to debug:
```shell
docker exec -it xv6 gdb-multiarch
```
