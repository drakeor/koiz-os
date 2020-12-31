# TODO: Eventually turn this into a Makefile...

# Make sure these folders exist
mkdir -p ../bin
mkdir -p ../obj
mkdir -p ../obj/core
mkdir -p ../obj/drivers
mkdir -p ../obj/libc

# Build our custom kernel entry executable
echo "compiling 32-bit kernel asm code"
fasm.x64 kernel_entry.asm ../obj/kernel_entry.o
fasm.x64 core/x86_idt.asm ../obj/core/x86_idt.o

# Compile the kernel
echo "compiling 32-bit kernel c code"
gcc -g -m32 -ffreestanding -c main.c -o ../obj/main.o -fno-pie
gcc -g -m32 -ffreestanding -c core/interrupt_handler.c -o ../obj/core/interrupt_handler.o -fno-pie
gcc -g -m32 -ffreestanding -c drivers/basic_io.c -o ../obj/drivers/basic_io.o -fno-pie
gcc -g -m32 -ffreestanding -c drivers/display.c -o ../obj/drivers/display.o -fno-pie
gcc -g -m32 -ffreestanding -c drivers/serial.c -o ../obj/drivers/serial.o -fno-pie
gcc -g -m32 -ffreestanding -c libc/stdlib.c -o ../obj/libc/stdlib.o -fno-pie

# Link everything together
# It is IMPORTANT that KERNEL_ENTRY.O is first!!
echo "linking 32-bit kernel"
ld -o ../bin/kernel.elf -Ttext 0x1000 ../obj/kernel_entry.o \
../obj/core/interrupt_handler.o \
../obj/core/x86_idt.o \
../obj/main.o \
../obj/drivers/basic_io.o \
../obj/drivers/display.o \
../obj/drivers/serial.o \
../obj/libc/stdlib.o \
-m elf_i386
#--oformat binary 

# Copy across elf file to bin
# We use the elf file for debugging
objcopy -O binary ../bin/kernel.elf ../bin/kernel.bin