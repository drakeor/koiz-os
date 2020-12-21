# KoiZ Operating System
Built mostly as a pet project. It's an x86_64 based operating system.
Built in Rust and ASM

# Requirements to build
- Rust
- FASM (https://flatassembler.net/download.php)

# Running
- You can run this OS bare metal or on QEMU (qemu-x86_64). You'll probably need to pass -fdr to QEMU to get it to work.

# FAQ

## FASM gives me an exec format error
Most likely have to run fasm.x64 instead

# Useful references
GDT: https://en.wikipedia.org/wiki/Global_Descriptor_Table
OS Dev Book: https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf
X86 Assembly Reference: https://en.wikibooks.org/wiki/X86_Assembly
OS Dev.org: https://wiki.osdev.org/Main_Page
FASM Manual: https://flatassembler.net/docs.php?article=manual