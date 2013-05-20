OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

SECTIONS
{

	. = 0x00000000;
	.init : {
		*(.init)
	}
	.text : {
		. = ALIGN(4);
		*(.text)
	}
	.data 0x08000000 : AT(ADDR(.text) + SIZEOF(.text)) {
		. = ALIGN(4);
		*(.data)
	}
	. = LOADADDR(.data) + SIZEOF(.data);
	.rodata : {
		. = ALIGN(4);
		*(.rodata)
	}
	. = ADDR(.data) + SIZEOF(.data);
	.bss : {
		. = ALIGN(4);
		*(.bss)
	}

	BSS_START = ADDR(.bss);
	BSS_END = ADDR(.bss) + SIZEOF(.bss);
	MEMORY_END = 0x09000000;
}
