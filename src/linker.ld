ENTRY(start)

/* Defini os mapas, tenho que conferir depois */

SECTIONS
{
	. = 1M;

	.text :
	{
		KEEP(*(.multiboot))
		*(.text)
	}

	.rodata :
	{
		*(.rodata)
	}

	.data :
	{
		*(.data)
	}

	.bss :
	{
		*(.COMMON)
		*(.bss)
	}
}
