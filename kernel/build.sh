# TODO: Eventually turn this into a Makefile...

# Make sure these folders exist
mkdir -p ../bin
mkdir -p ../obj
mkdir -p ../obj/drivers
mkdir -p ../obj/drivers/io
mkdir -p ../obj/drivers/irq
mkdir -p ../obj/drivers/memory
mkdir -p ../obj/drivers/serial
mkdir -p ../obj/drivers/video
mkdir -p ../obj/libc
mkdir -p ../obj/tests

echo "compiling bootloader asm code"
#as --32 -nostdlib boot/boot.s -o ../obj/boot/boot.o
fasm.x64 boot/boot.asm ../obj/boot/boot.o
fasm.x64 boot/gdt.asm ../obj/boot/gdt.o

# Build our custom kernel entry executable
echo "compiling 32-bit kernel asm code"
#fasm.x64 kernel_entry.asm ../obj/kernel_entry.o
#fasm.x64 core/x86_pagedir.asm ../obj/core/x86_pagedir.o
fasm.x64 drivers/irq/interrupt_handler.asm ../obj/drivers/irq/interrupt_handler.o

# Compile the kernel
echo "compiling 32-bit kernel c code"
gcc -g -m32 -ffreestanding -mno-red-zone -c kernel_main.c -o ../obj/kernel_main.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/io/basic_io.c -o ../obj/drivers/io/basic_io.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/irq/idt_setup.c -o ../obj/drivers/irq/idt_setup.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/serial/serial.c -o ../obj/drivers/serial/serial.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c drivers/video/vga.c -o ../obj/drivers/video/vga.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c libc/stdlib.c -o ../obj/libc/stdlib.o -fno-pie
gcc -g -m32 -ffreestanding -mno-red-zone -c tests/interrupt_tests.c -o ../obj/tests/interrupt_tests.o -fno-pie

# Link everything together
# It is IMPORTANT that boot.o is first!!
echo "linking 32-bit kernel"
gcc -T linker.ld -o ../bin/koizos-grub.bin -ffreestanding -nostdlib -m32 \
../obj/boot/boot.o \
../obj/boot/gdt.o \
../obj/drivers/io/basic_io.o \
../obj/drivers/irq/idt_setup.o \
../obj/drivers/irq/interrupt_handler.o \
../obj/drivers/serial/serial.o \
../obj/drivers/video/vga.o \
../obj/libc/stdlib.o \
../obj/tests/interrupt_tests.o \
../obj/kernel_main.o

# Ensure that the final image is a grub multiboot file
grub-file --is-x86-multiboot ../bin/koizos-grub.bin
if [ $? -ne 0 ]
then
    printf "kernel image is not a multiboot grub file!\n"
    exit 1
else
    printf "kernel image is a valid multiboot grub file\n"
fi

# Print final image size
myfilesize=$(wc -c "../bin/koizos-grub.bin" | awk '{print $1}')
printf "final kernel image size: %d\n" $myfilesize

# Build iso image
printf "Building ISO image\n"
mkdir -p ../bin/isodir/boot/grub
cp ../bin/koizos-grub.bin ../bin/isodir/boot/koizos.bin
cp boot/grub.cfg ../bin/isodir/boot/grub/grub.cfg
grub-mkrescue -o ../bin/koizos.iso ../bin/isodir

# Print final iso image size
myfilesize=$(wc -c "../bin/koizos.iso" | awk '{print $1}')
printf "final iso image size: %d\n" $myfilesize