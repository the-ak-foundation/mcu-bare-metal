#include <stdint.h>

int main(void);

extern void _estack(void);

__attribute__((naked, noreturn)) void Reset_Handler(void)
{
	extern long _sbss, _ebss;
	extern long _sdata, _edata;
	extern long _sidata;

	for (long* dst = &_sbss; dst < &_ebss; dst++)
	{
		*dst = 0;
	}

	for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;)
	{
		*dst++ = *src++;
	}

	main();

	for (;;)
	{
		(void)0;
	}
}

__attribute__((section(".isr_vector"))) void (*const g_pfnVectors[16 + 45])(void) = {
    _estack,
    Reset_Handler,
};

int main(void)
{
	*((volatile uint32_t*)0xE000ED08) = 0x08003000;

	for (;;)
	{
	}
	return 0;
}