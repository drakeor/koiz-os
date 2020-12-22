# Make sure these folders exist
mkdir -p ../bin
mkdir -p ../obj

echo "compiling kernel entry code"
# Build our custom kernel entry executable
fasm.x64 kernel_entry.asm ../obj/kernel_entry.o

echo "compiling kernel"
# Compile the kernel
gcc -m32 -ffreestanding -c main.c -o ../obj/main.o -fno-pie

echo "linking kernel"
# Link everything together
ld -o ../bin/kernel.bin -Ttext 0x1000 ../obj/kernel_entry.o ../obj/main.o --oformat binary -m elf_i386
