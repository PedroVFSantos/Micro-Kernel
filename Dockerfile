FROM ubuntu:22.04

LABEL maintainer="Pedro V. F. Santos"
LABEL description="Ambiente de Build para Microkernel (GCC+GRUB+XORRISO)"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-multilib \
    g++-multilib \
    nasm \
    xorriso \
    mtools \
    grub-pc-bin \
    grub-common \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root/env
VOLUME /root/env
CMD ["make", "iso"]