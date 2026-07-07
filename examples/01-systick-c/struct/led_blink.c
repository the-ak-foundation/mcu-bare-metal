#include "led_blink.h"

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

int main(void)
{
	RCC->AHBENR |= (1U << 1);
	GPIOB->MODER |= (1U << (LED_PIN * 2));

	SysTick->LOAD = (SYSCLK_HZ / TICK_HZ) - 1U;
	SysTick->VAL = 0U;
	SysTick->CTRL = (1U << 0) | (1U << 1) | (1U << 2);

	uint32_t last_tick = g_tick;

	for (;;)
	{
		if (g_tick - last_tick >= 100)
		{
			last_tick = g_tick;
			GPIOB->ODR ^= (1U << LED_PIN);
		}
	}
	return 0;
}
