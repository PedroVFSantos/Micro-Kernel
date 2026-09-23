KERNEL = kernel.bin
ISO = microkernel.iso
BUILD = build

CXX = g++
NASM = nasm
AS = gcc
LD = ld

CPP_SRC = $(wildcard src/*.cpp)
OBJ = $(BUILD)/boot.o $(BUILD)/gdt_flush.o $(BUILD)/interrupts.o $(BUILD)/switch.o \
      $(patsubst src/%.cpp,$(BUILD)/%.o,$(CPP_SRC))

# freestanding 32-bit: sem libc, sem exceções/RTTI, sem statics thread-safe.
# -mgeneral-regs-only porque o kernel não habilita SSE/FPU e o GCC gosta de vetorizar loops.
# -fno-pie/-fno-stack-protector porque algumas distros ligam isso por padrão.
CXXFLAGS = -m32 -std=gnu++17 -ffreestanding -fno-exceptions -fno-rtti -fno-threadsafe-statics \
           -fno-pie -fno-stack-protector -mgeneral-regs-only -fno-asynchronous-unwind-tables \
           -O2 -Wall -Wextra -Iinclude -MMD -MP
LDFLAGS = -m elf_i386 -T src/linker.ld -nostdlib

QEMU = qemu-system-i386
QEMU_FLAGS = -m 128M -serial stdio

all: $(KERNEL)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: src/boot.s | $(BUILD)
	$(AS) -m32 -c $< -o $@

$(BUILD)/%.o: src/%.s | $(BUILD)
	$(NASM) -f elf32 $< -o $@

$(BUILD)/%.o: src/%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(KERNEL): $(OBJ) src/linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

iso: $(KERNEL)
	mkdir -p iso/boot/grub
	cp $(KERNEL) iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) iso

# Boot direto pelo loader multiboot do QEMU, não precisa de GRUB
run: $(KERNEL)
	$(QEMU) $(QEMU_FLAGS) -kernel $(KERNEL)

run-iso: iso
	$(QEMU) $(QEMU_FLAGS) -cdrom $(ISO)

clean:
	rm -rf $(BUILD) $(KERNEL) $(ISO) iso

-include $(OBJ:.o=.d)

.PHONY: all iso run run-iso clean
