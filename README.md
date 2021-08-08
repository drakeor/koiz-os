# KoiZ Operating System

Built mostly as a pet project. It's an x86_64 based general operating system built in C and ASM.

It currently only runs in 32-bit since I don't have all the code setup for 64-bit. 

It doesn't really serve any practical purpose outside that.

NOTE: For WSL2, add an inbound rule for TCP 6000!

# Completed Features

- Custom bootloader image to load the kernel (DONE)

- Prints stuff to the console in 32 bit mode (DONE)

- Write output to a serial port for logging (DONE)

- Starting libc functions (DONE)

- Adding Interrupts (DONE)

- Basic Keyboard support (DONE)

- Basic Terminal Support (DONE)

- Memory Detection (DONE)

- Physical Memory Management (DONE)

- Added paging (DONE)

- Virtual Memory Management (DONE)

- Virtual to physical address translation functions (DONE)

- Ramdisk for virtual hard drive (DONE)

- Huge refactor for Kernel organization + GRUB (DONE)

- Make GRUB compatiable (DONE)

- FAT16 filesystem on ramdisk (DONE)

- Add more standard lib stuff to stdlib.c, strings.c (DONE)

# In Progress Features

- Kernel Standard IO Streams 

- Slab Allocator / KMalloc / KFree 

- Add basic shell

# Planned Features (in order of development)

- File management

- Add system calls

- Userspace application in filesystem

- Read from an actual disk drive

- Read a FAT32 system from an actual disk drive (or emulated)

- Network Driver

- PCI scanning

- Userspace application

- Handling userspace page faults

- Processes

- Backend Compiler for OS

- TCP/IP

- GUI

- Multi-processor support

- Implement FPU / SSE stuff

# Requirements to build

- FASM (https://flatassembler.net/download.php)

- GCC (Supports at least C99 standard)

- grub (apt-get install grub grub-pc-bin)

- xorriso (apt-get install xorriso)


# Running

- You can (theoretically) run this OS bare metal or on QEMU (qemu-x86_64). You'll probably need to pass -fda to QEMU to get it to work.

- The kernel needs at bare minimum 4MB of RAM to start. It will fail with anything less.

# Known Issues

- Will triple fault on some modern bare metal machines. Not guaranteed to work on yours.

- No checks for kernel stack to start writing into kernel code. Obviously stability and security issue.

# Memory Layout

- 0x0000    - Free memory
- 0x1000    - Old Kernel Entry Point
- 0x7c00    - Bootloader location
- 0x8000    - BIOS Memory Map Size Location (bootsector/detectmem.asm)
- 0x8004    - BIOS Memory Map Location (bootsector/detectmem.asm)
- 0x10000   - New Kernel Entry Point
- 0x80000   - Kernel Stack Start
- 0xb8000   - Video memory 
- 0x100000  - Identity Mapped Free Memory start

# FAQ

## FASM gives me an exec format error

Most likely have to run fasm.x64 instead

## Can I use this for my project?

If it's to reference to learn/develop your own OS, I encourage it! If you want to use this on an actual production process, I discourage it. I can't guarantee the stability and security of this operating system.

# Useful references / credits

- OS Dev Book (Does a pretty good job explaining theory. Used this heavily in beginning my OS): https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

- X86 Assembly Reference (We can't avoid assembly for parts of this OS): https://en.wikibooks.org/wiki/X86_Assembly

- OS Dev.org (Lots of great information and resource): https://wiki.osdev.org/Main_Page

- FASM Manual (Incredibly helpful since most snippets are built in NASM and I like using FASM: https://flatassembler.net/docs.php?article=manual

- In-line assembly docs for GCC (Since the syntax can be awkward sometimes, at least for me): https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html#Using-Assembly-Language-with-C 

- Kernel.org Coding Style (I'm not programming the linux kernel so it's not strictly followed, but my coding style is similar. I do really enjoy the rationale being explained in this document though): https://www.kernel.org/doc/Documentation/process/coding-style.rst 

- Linux Kernel (Helps to see what people much better at this than me are doing): https://github.com/torvalds/linux 