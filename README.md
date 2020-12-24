# KoiZ Operating System

Built mostly as a pet project. It's an x86_64 based operating system built in C, Rust and ASM.

It currently runs 32-bit mode with plans to have a 64-bit mode as well.

A lot of programs and stuff revolve around math since I like math. 

It doesn't really serve any practical purpose outside that.


# Completed Features

- Custom bootloader image to load the kernel (DONE)

- Prints stuff to the console in 32 bit mode (DONE)


# Planned Features (in order of development)

- Write output to a serial port for debugging (IN PROGRESS)

- Starting libc functions

- Adding self-tests

- Adding Interrupts

- Malloc 

- Shell

- Keyboard support

- Userspace application

- Processes

- Backend Compiler for OS


# Requirements to build

- FASM (https://flatassembler.net/download.php)

- GCC (Supports at least C99 standard)

- LD


# Running

- You can run this OS bare metal or on QEMU (qemu-x86_64). You'll probably need to pass -fda to QEMU to get it to work.


# FAQ

## FASM gives me an exec format error

Most likely have to run fasm.x64 instead


# Useful references / credits

- OS Dev Book (Does a pretty good job explaining theory. Used this heavily in beginning my OS): https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

- X86 Assembly Reference (We can't avoid assembly for parts of this OS): https://en.wikibooks.org/wiki/X86_Assembly

- OS Dev.org (Lots of great information and resource): https://wiki.osdev.org/Main_Page

- FASM Manual (Incredibly helpful since most snippets are built in NASM and I like using FASM: https://flatassembler.net/docs.php?article=manual

- In-line assembly docs for GCC (Since the syntax can be awkward sometimes, at least for me): https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html#Using-Assembly-Language-with-C 

- Kernel.org Coding Style (I'm not programming the linux kernel so it's not strictly followed, but my coding style is similar. I do really enjoy the rationale being explained in this document though.): https://www.kernel.org/doc/Documentation/process/coding-style.rst 

- Linux Kernel (Helps to see what people much better at this than me are doing): https://github.com/torvalds/linux 