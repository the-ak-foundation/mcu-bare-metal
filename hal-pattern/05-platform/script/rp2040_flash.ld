ENTRY(Reset_Handler)

MEMORY
{
	FLASH (rx)  : ORIGIN = 0x10000000, LENGTH = 2048K
	SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 256K
}

_estack = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS
{
	.boot2 :
	{
		__boot2_start__ = .;
		KEEP(*(.boot2))
		. = __boot2_start__ + 256;
		__boot2_end__ = .;
	} > FLASH

	.isr_vector :
	{
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} > FLASH

	.text :
	{
		. = ALIGN(4);
		*(.text*)
		KEEP(*(.init))
		KEEP(*(.fini))
		*(.rodata*)
		. = ALIGN(4);
		_etext = .;
	} > FLASH

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH

	.ARM.exidx :
	{
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} > FLASH

	.preinit_array :
	{
		PROVIDE_HIDDEN(__preinit_array_start = .);
		KEEP(*(.preinit_array*))
		PROVIDE_HIDDEN(__preinit_array_end = .);
	} > FLASH

	.init_array :
	{
		PROVIDE_HIDDEN(__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array*))
		PROVIDE_HIDDEN(__init_array_end = .);
	} > FLASH

	.fini_array :
	{
		PROVIDE_HIDDEN(__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array*))
		PROVIDE_HIDDEN(__fini_array_end = .);
	} > FLASH

	_sidata = LOADADDR(.data);

	.data :
	{
		. = ALIGN(4);
		_sdata = .;
		*(.data*)
		. = ALIGN(4);
		_edata = .;
	} > SRAM AT > FLASH

	.bss (NOLOAD) :
	{
		. = ALIGN(4);
		_sbss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
	} > SRAM

	. = ALIGN(8);
	_end = .;
	PROVIDE(end = .);
}
