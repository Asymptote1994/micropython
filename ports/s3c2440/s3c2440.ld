OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

SECTIONS {
    . = 0x30000000;

    . = ALIGN(4);

    .text :
    { 
        *(.text) 
        *(.text*)
    }

	. = ALIGN(4);

    .rodata :
    { 
        *(.rodata) 
        *(.rodata*)
    }
	
	. = ALIGN(4);

    .data :
    { 
        *(.data) 
        *(.data*)
    }
     
	. = ALIGN(4);

	__bss_start = .;
    .bss :
    { 
        *(.bss)
        *(.bss*)
        *(COMMON)
    }
    __bss_end = .;
}