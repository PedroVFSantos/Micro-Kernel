KERNEL=kernel.bin
OBJ=boot.o kernel.o 
CXX = g++
AS = gcc
LD = ld
CXXFLAGS = -std=gnu++17 -ffreestanding -fno-exceptions -fno-rtti -O2 -Iinclude
LDFLAGS = -T src/linker.ld -nostdlib

all: $(KERNEL) iso

boot.o: src/boot.s
	$(AS) -c src/boot.s -o boot.o

kernel.o: src/kernel.cpp
	$(CXX) $(CXXFLAGS) -c src/kernel.cpp -o kernel.o

console.o: src/console.cpp
	$(CXX) $(CXXFLAGS) -c src/console.cpp -o console.o

scheduler.o: src/scheduler.cpp
	$(CXX) $(CXXFLAGS) -c src/scheduler.cpp -o scheduler.o

ipc.o: src/ipc.cpp
	$(CXX) $(CXXFLAGS) -c src/ipc.cpp -o ipc.o

user_tasks.o: src/user_tasks.cpp
	$(CXX) $(CXXFLAGS) -c src/user_tasks.cpp -o user_tasks.o

$(KERNEL): $(OBJ)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJ)

iso: $(KERNEL)
	mkdir -p iso/boot/grub
	cp $(KERNEL) iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o microkernel.iso iso

run: iso
	qemu-system-x86_64 -cdrom microkernel.iso -m 512M

clean:
	rm -f *.o $(KERNEL) microkernel.iso
	rm -rf iso

.PHONY: all iso run clean

