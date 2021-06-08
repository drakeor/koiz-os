# TODO: Eventually turn this into a Makefile...

# Make sure these folders exist
mkdir -p ../bin
mkdir -p ../obj
mkdir -p ../obj/core
mkdir -p ../obj/drivers
mkdir -p ../obj/drivers/fs_backend
mkdir -p ../obj/libc

# Build our custom kernel entry executable
echo "compiling 32-bit kernel asm code"
fasm.x64 kernel_entry.asm ../obj/kernel_entry.o
fasm.x64 core/x86_pagedir.asm ../obj/core/x86_pagedir.o
fasm.x64 core/x86_idt.asm ../obj/core/x86_idt.o

# Compile the kernel
echo "compiling 32-bit kernel c code"
gcc -g -m32 -ffreestanding -mno-red-zone -c main.c -o ../obj/main.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c core/interrupt_handler.c -o ../obj/core/interrupt_handler.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c core/mem.c -o ../obj/core/mem.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c core/pic.c -o ../obj/core/pic.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c core/vmem.c -o ../obj/core/vmem.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/fs_backend/ramdisk.c -o ../obj/drivers/fs_backend/ramdisk.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/basic_io.c -o ../obj/drivers/basic_io.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/display.c -o ../obj/drivers/display.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/filesystem.c -o ../obj/drivers/filesystem.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/keyboard.c -o ../obj/drivers/keyboard.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/network.c -o ../obj/drivers/network.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/serial.c -o ../obj/drivers/serial.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/tty.c -o ../obj/drivers/tty.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c libc/stdlib.c -o ../obj/libc/stdlib.o -fno-pie

# Link everything together
# It is IMPORTANT that KERNEL_ENTRY.O is first!!
echo "linking 32-bit kernel"
ld -o ../bin/kernel.elf -Ttext 0x1000 ../obj/kernel_entry.o \
../obj/core/interrupt_handler.o \
../obj/core/mem.o \
../obj/core/pic.o \
../obj/core/x86_pagedir.o \
../obj/core/x86_idt.o \
../obj/core/vmem.o \
../obj/main.o \
../obj/drivers/fs_backend/ramdisk.o \
../obj/drivers/basic_io.o \
../obj/drivers/display.o \
../obj/drivers/filesystem.o \
../obj/drivers/keyboard.o \
../obj/drivers/network.o \
../obj/drivers/serial.o \
../obj/drivers/tty.o \
../obj/libc/stdlib.o \
-m elf_i386
#--oformat binary 

# Get rid of the annoying GNU section that bloats this file
cp ../bin/kernel.elf ../bin/kernel2.elf
strip --remove-section=.note.gnu.property ../bin/kernel2.elf

# Copy across elf file to bin
# We use the elf file for debugging
objcopy -O binary ../bin/kernel2.elf ../bin/kernel.bin

# Verify the filesize is under 25KiB (So it doesn't overwrite the bootloader)
# Memory kernel occupies is 0x1000 - 0x7BFF
myfilesize=$(wc -c "../bin/kernel.bin" | awk '{print $1}')
printf "final kernel image size: %d\n" $myfilesize
if [ $myfilesize -gt 25000 ]
then
    printf "Build error!! kernel image will overwrite bootloader!\n"
    exit 1
fi
