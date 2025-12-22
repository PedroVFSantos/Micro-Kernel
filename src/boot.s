.set ALIGN,    1<<0             
.set MEMINFO,  1<<1             
.set FLAGS,    ALIGN 
.set MAGIC,    0x1BADB002       
.set CHECKSUM, -(MAGIC + FLAGS) 

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

#Stack (Setei para 16kb, porém pode ser alterado)
.section .bss
.align 16
stack_bottom:
.skip 16384 
stack_top:

.section .text
.global start
.type start, @function
start:
	mov $stack_top, %esp

	call kernel_main

	cli
1:	hlt
	jmp 1b

.size start, . - start
