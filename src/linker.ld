ENTRY(start)

SECTIONS
{
	. = 1M;

	.text :
	{
		KEEP(*(.multiboot))
		*(.text .text.*)
	}

	.rodata :
	{
		*(.rodata .rodata.*)
	}

	.data :
	{
		*(.data .data.*)
	}

	.bss :
	{
		*(COMMON)
		*(.bss .bss.*)
	}

	/* PMM usa isso pra saber até onde o kernel ocupa memória */
	. = ALIGN(4K);
	_kernel_end = .;

	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note .note.*)
	}
}
