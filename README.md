# KoiZ Operating System

Built mostly as a pet project. It's an x86_64 based operating system built in Rust and ASM.

A lot of programs and stuff revolve around math since I like math. 

It doesn't really serve any practical purpose outside that.

# Requirements to build

- Rust (Nightly required. Can use `rustup default nightly` to install it)

- Phil Opp's Rust BootImage (`cargo install bootimage`)

- LLVM Tools (`rustop component add llvm-tools-preview`)

- FASM (https://flatassembler.net/download.php)

# Running

- You can run this OS bare metal or on QEMU (qemu-x86_64). You'll probably need to pass -fdr to QEMU to get it to work.

# FAQ

## FASM gives me an exec format error

Most likely have to run fasm.x64 instead


# Useful references / credits

OS Dev Book (Does a pretty good job explaining theory): https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

X86 Assembly Reference: https://en.wikibooks.org/wiki/X86_Assembly

OS Dev.org (Lots of great information and resource): https://wiki.osdev.org/Main_Page

FASM Manual (Incredibly helpful since most snippets are built in NASM and it's always 1 for 1): https://flatassembler.net/docs.php?article=manual

ReDox (A very complete OS made in Rust): https://github.com/redox-os/redox

Phil Opp (Another dude who build an OS in Rust. His site helped a lot for programming free-standing binaries): https://os.phil-opp.com/
