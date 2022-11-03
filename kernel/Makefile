# Project variables
TARGET = koizos
OBJDIR = ../obj
BINDIR = ../bin

# GCC
CC = gcc
CFLAGS = -Werror -g -m32 -ffreestanding -mno-red-zone -Wall

# FASM
ASMC = fasm.x64
ASMFLAGS =

# Linker
LD = $(CC)
LDFLAGS = -T linker.ld -o $(BINDIR)/$(TARGET).bin -ffreestanding -nostdlib -m32

# Set up sources and objects
CSOURCES = $(shell find * -type f -iname '*.c') 
ASMSOURCES = $(shell find * -type f -iname '*.asm') 

ALLSOURCES = $(shell find * -type f -iname '*.c' -o -type f -iname '*.asm') 
OBJECTS = $(foreach x, $(basename $(ALLSOURCES)), $(OBJDIR)/$(x).o)

# All targets
all: $(TARGET)
	mkdir -p $(BINDIR)/isodir/boot/grub
	cp $(BINDIR)/$(TARGET).bin $(BINDIR)/isodir/boot/$(TARGET).bin
	cp boot/grub.cfg $(BINDIR)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(BINDIR)/$(TARGET).iso $(BINDIR)/isodir

# Compile C into O
$(OBJDIR)/%.o: %.c
	mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile ASM into O
$(OBJDIR)/%.o: %.asm
	mkdir -p $(shell dirname $@)
	$(ASMC) $(ASMFLAGS) $< $@

# Link everything together
$(TARGET): $(OBJECTS)
	mkdir -p $(BINDIR)
	$(LD) -o $@ $(LDFLAGS) $^ 

# Clean everything
clean:
	rm -rf $(BINDIR) 
	rm -rf $(OBJDIR)