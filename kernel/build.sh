# Make sure these folders exist
mkdir -p ../bin
mkdir -p ../obj

echo "compiling 32-bit kernel entry code"
# Build our custom kernel entry executable
fasm.x64 kernel_entry.asm ../obj/kernel_entry.o

echo "compiling 32-bit kernel"
# Compile the kernel
gcc -m32 -ffreestanding -c main.c -o ../obj/main.o -fno-pie

echo "linking 32-bit kernel"
# Link everything together
ld -o ../bin/kernel.bin -Ttext 0x1000 ../obj/kernel_entry.o ../obj/main.o --oformat binary -m elf_i386
