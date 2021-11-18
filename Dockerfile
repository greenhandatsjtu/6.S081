FROM ubuntu:20.04
ARG arch_name=amd64

ENV TZ=Asia/Shanghai \
    LANG=en_US.utf8 \
    LANGUAGE=en_US.UTF-8 \
    LC_ALL=en_US.UTF-8 \
    DEBIAN_FRONTEND=noninteractive


RUN apt-get update && \
    apt-get install -y git curl vim build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu libpixman-1-dev gcc-riscv64-unknown-elf libglib2.0-dev pkg-config

RUN curl -LJO https://download.qemu.org/qemu-5.1.0.tar.xz
RUN tar xf qemu-5.1.0.tar.xz
RUN cd qemu-5.1.0 && \
    ./configure --disable-kvm --disable-werror --prefix=/usr/local --target-list=riscv64-softmmu && \
    make && \
    make install

VOLUME xv6
WORKDIR xv6

CMD [ "bash" ]