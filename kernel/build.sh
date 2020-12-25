# TODO: Eventually turn this into a Makefile...

# Make sure these folders exist
mkdir -p ../bin
mkdir -p ../obj
mkdir -p ../obj/drivers
mkdir -p ../obj/libc

# Build our custom kernel entry executable
echo "compiling 32-bit kernel entry code"
fasm.x64 kernel_entry.asm ../obj/kernel_entry.o

# Compile the kernel
echo "compiling 32-bit kernel"
gcc -m32 -ffreestanding -c main.c -o ../obj/main.o -fno-pie
gcc -m32 -ffreestanding -c drivers/basic_io.c -o ../obj/drivers/basic_io.o -fno-pie
gcc -m32 -ffreestanding -c drivers/display.c -o ../obj/drivers/display.o -fno-pie
gcc -m32 -ffreestanding -c drivers/serial.c -o ../obj/drivers/serial.o -fno-pie
gcc -m32 -ffreestanding -c libc/stdio.c -o ../obj/libc/stdio.o -fno-pie

# Link everything together
echo "linking 32-bit kernel"
ld -o ../bin/kernel.bin -Ttext 0x1000 ../obj/kernel_entry.o \
../obj/main.o \
../obj/drivers/basic_io.o \
../obj/drivers/display.o \
../obj/drivers/serial.o \
../obj/libc/stdio.o \
--oformat binary -m elf_i386
