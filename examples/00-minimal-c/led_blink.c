#include "led_blink.h"

int main(void);
extern void _estack(void);

__attribute__((naked, noreturn)) void Reset_Handler(void)
{
	extern long _sbss, _ebss, _sdata, _edata, _sidata;

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

__attribute__((section(".isr_vector"))) void (*const g_pfnVectors[2])(void) = {
    _estack,
    Reset_Handler,
};

static void delay(volatile uint32_t count)
{
	while (count--)
	{
		__asm__("nop");
	}
}

int main(void)
{
	RCC_AHBENR |= (1U << 1);
	GPIOB_MODER |= (1U << (LED_PIN * 2));

	for (;;)
	{
		GPIOB_BSRR = (1U << LED_PIN);
		delay(100000);
		GPIOB_BSRR = (1U << (LED_PIN + 16));
		delay(100000);
	}
	return 0;
}