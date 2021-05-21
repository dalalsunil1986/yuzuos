ENTRY(_start)

SECTIONS
{
	. = 0xC0100000;
	kernel_start = .;
	.text ALIGN(4096) : AT(ADDR(.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}
	.rodata ALIGN(4096) : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata)
	}
	.data ALIGN(4096) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}
	.bss ALIGN(4096) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}
	kernel_end = .;
}
