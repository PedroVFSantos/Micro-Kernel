KERNEL=kernel.bin
# Lista completa de objetos necessários para o Linker
OBJ=boot.o kernel.o console.o gdt.o gdt_flush.o idt.o interrupts.o pmm.o scheduler.o ipc.o switch.o user_tasks.o

CXX = g++
NASM = nasm
AS = gcc
LD = ld

# Flags para ambiente freestanding 32-bit e sem inicialização segura de statics
CXXFLAGS = -m32 -std=gnu++17 -ffreestanding -fno-exceptions -fno-rtti -fno-threadsafe-statics -O2 -Iinclude
LDFLAGS = -m elf_i386 -T src/linker.ld -nostdlib

all: $(KERNEL) iso

# Objetos em Assembly (GAS)
boot.o: src/boot.s
	$(AS) -m32 -c src/boot.s -o boot.o

# Objetos em Assembly (NASM)
gdt_flush.o: src/gdt_flush.s
	$(NASM) -f elf32 src/gdt_flush.s -o gdt_flush.o

interrupts.o: src/interrupts.s
	$(NASM) -f elf32 src/interrupts.s -o interrupts.o

switch.o: src/switch.s
	$(NASM) -f elf32 src/switch.s -o switch.o

# Regra genérica para todos os arquivos .cpp em src/
%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linkagem final [cite: 4]
$(KERNEL): $(OBJ)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJ)

# Geração da ISO bootável 
iso: $(KERNEL)
	mkdir -p iso/boot/grub
	cp $(KERNEL) iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o microkernel.iso iso

clean:
	rm -f *.o $(KERNEL) microkernel.iso
	rm -rf iso

.PHONY: all iso clean