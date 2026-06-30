#include "bm_led_blink.h"

int main(void);
extern void _estack(void);

volatile uint32_t g_tick;

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
	}
}

void SysTick_Handler(void)
{
	g_tick++;
}

__attribute__((section(".isr_vector"))) void (*const g_pfnVectors[16])(void) = {
    _estack,
    Reset_Handler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    SysTick_Handler,
};

static void delay_ms(uint32_t ms)
{
	uint32_t start = g_tick;
	while ((g_tick - start) < ms)
	{
	}
}

int main(void)
{
	RCC_AHBENR |= (1U << 1);
	GPIOB_MODER |= (1U << (LED_PIN * 2));

	SYSTICK_LOAD = (SYSCLK_HZ / TICK_HZ) - 1U;
	SYSTICK_VAL = 0U;
	SYSTICK_CTRL = (1U << 0) | (1U << 1) | (1U << 2);

	for (;;)
	{
		GPIOB_BSRR = (1U << LED_PIN);
		delay_ms(100);
		GPIOB_BSRR = (1U << (LED_PIN + 16));
		delay_ms(100);
	}
	return 0;
}
