**Note that my initial goal was to get to userspace processes, which I accomplished in November 2022. I'm currently not planning any major features, though I may come back to document and clean up parts of this repository**


# KoiZ Operating System

Built mostly as an educational pet project. It's an x86_64 based general operating system built in C and ASM. Most of the code and implementations inside are written by me. Code that is not mine is labeled as such. 

It currently only runs in 32-bit since I don't have all the code setup for 64-bit. 

NOTE: For WSL2, add an inbound rule for TCP 6000!


# Completed Feature Milestones

## 1. Bootloader and Starting Out

![milestone1_image](https://drakeor.com/content/images/2022/04/standard_lib_vid.gif)

- Custom bootloader image to load the kernel (DONE)

- Prints stuff to the console in 32 bit mode (DONE)

- Write output to a serial port for logging (DONE)

- Starting libc functions (DONE)

- Adding Interrupts (DONE)

- Basic Keyboard support (DONE)

- Basic Terminal Support (DONE)

## 2. Physical and Virtual Memory Management

![milestone2_image](https://drakeor.com/content/images/2022/04/GIF-11-10-2021-12-47-40-AM--1-.gif)

- Memory Detection (DONE)

- Physical Memory Management (DONE)

- Added paging (DONE)

- Virtual Memory Management (DONE)

- Virtual to physical address translation functions (DONE)

## 3. Standard IO Streams / Heap

![milestone3_image](https://drakeor.com/content/images/2023/12/malloc.gif)

- Add basic shell connected to input (DONE)

- Kernel Standard IO Streams (DONE)

- Slab Allocator / KMalloc / KFree (DONE)

## 4. Simple Filesystem

![milestone4_image](https://drakeor.com/content/images/2025/11/GIF-12-28-2023-10-20-27-PM.gif)

- Ramdisk for virtual hard drive (DONE)

- Huge refactor for Kernel organization + GRUB (DONE)

- Make GRUB compatiable (DONE)

- FAT16 filesystem on ramdisk (DONE)

- Add more standard lib stuff to stdlib.c, strings.c (DONE)

- Add basic commands (DONE)

- Simple Reading and Writing File management commands (DONE)

## 5. User Space

![milestone5_image](https://drakeor.com/content/images/2025/11/Screencast-from-2025-11-26-15-13-58-1.gif)

- Implement TSS (DONE)

- Allow dropping to User Mode (DONE)

- Add system calls (DONE)

## 6. Processes

![milestone6_image](https://drakeor.com/content/images/2025/11/Screencastfrom2025-11-2700-15-59-ezgif.com-optimize-1.gif)

- Add Timer interrupts (DONE)

- Add processes  (DONE)

- Add process scheduler  (DONE)

- Userspace application in filesystem  (DONE)


# Potential Future Work

## 8. Robust Processes

- Better support for userspace applications
  
- Handling userspace page faults

## 8. GUI

- Add math library functions

- Add VESA Graphics

- Add Gfx drawing for OS GUI

- Read from an actual disk drive

## 9. Connectivity

- Read a FAT32 system from an actual disk drive (or emulated)

- TCP/IP

- Network Driver

- PCI scanning

## 10. Enhanced Features

- Multi-processor support

- Implement FPU / SSE stuff

- Backend Compiler for OS

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
