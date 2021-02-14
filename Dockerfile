FROM ubuntu:18.04

RUN apt-get update

# Install some prerequisites
RUN apt-get install -y gcc
RUN apt-get install -y binutils
RUN apt-get install -y qemu-system-x86
RUN apt-get install -y xvfb
RUN apt-get install -y gdb

# Install FASM
COPY ./scripts/docker/fasm.tgz /
RUN tar -xzf /fasm.tgz -C /usr/bin/
RUN rm /fasm.tgz

RUN mkdir -p /space/koiz-os

WORKDIR /space/koiz-os