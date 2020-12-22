mkdir -p ../bin
mkdir -p ../obj
gcc -m32 -ffreestanding -c main.c -o ../obj/main.o -fno-pie
ld -o ../bin/kernel.bin -Ttext 0x1000 ../obj/main.o --oformat binary -m elf_i386