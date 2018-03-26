OUTPUT_FORMAT("elf32-littlearm","elf32-bigarm","elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY{

	FLASH (rx) : org = 0, l = 1M
	RAM (rwx) : org = 0x8000000, l = 16M

}

SECTIONS{

  .vectors : { *(.vectors)} > FLASH


  . = 0x8000000;
  
  .init : { *(.init)} > RAM

  .text : { *(.text)} > RAM 
  
  
  .data : { 
  	*(.data)
  	. = ALIGN(4);
  } > RAM AT> FLASH
  
  START_DATA = ADDR(.data);
  END_DATA = START_DATA + SIZEOF(.data);
  
  START_FLASH_DATA = LOADADDR(.data);
  END_FLASH_DATA = START_FLASH_DATA + SIZEOF(.data);
  
  
  .bss : { 
  	*(.bss)
  	. = ALIGN(4);
  } > RAM
  START_BSS = ADDR(.bss);
  END_BSS = ADDR(.bss) + SIZEOF(.bss);
  
  
  
  .rodata : { 
  	*(.rodata)
  	. = ALIGN(4);
  } > RAM
}


