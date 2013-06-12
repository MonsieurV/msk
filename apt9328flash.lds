OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

_start = 0x00000000;

MEMORY
{
    ROM (rx) : ORIGIN = 0, LENGTH = 16M
    RAM (wx) : ORIGIN = 0x08000000, LENGTH = 1M
}


SECTIONS
{
    . = _start;
	.init : {
        . = ALIGN(4);
		*(.init)
	} > ROM
	.text : {
		. = ALIGN(4);
		*(.text)
	} > ROM
	.data : {
		. = ALIGN(4);
		*(.data)
	} > RAM AT> ROM
	.rodata : {
		. = ALIGN(4);
		*(.rodata)
	} > ROM
	.bss : {
		. = ALIGN(4);
		*(.bss)
	} > RAM

	BSS_START = ADDR(.bss);
	BSS_END = ADDR(.bss) + SIZEOF(.bss);
    DATA_RAM_START = ADDR(.data);
    DATA_RAM_END = ADDR(.data) + SIZEOF(.bss);
    DATA_FLASH_START = LOADADDR(.data);
    DATA_FLASH_END = LOADADDR(.data) + SIZEOF(.bss);
	MEMORY_END = 0x09000000;
}
