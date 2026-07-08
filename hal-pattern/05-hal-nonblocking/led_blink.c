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
	}
}

void SysTick_Handler(void)
{
	HAL_IncTick();
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
	HAL_Init();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio_init = {
	    .Pin = LED_PIN,
	    .Mode = GPIO_MODE_OUTPUT_PP,
	    .Pull = GPIO_NOPULL,
	    .Speed = GPIO_SPEED_FREQ_LOW,
	};
	HAL_GPIO_Init(LED_PORT, &gpio_init);

	uint32_t last_tick = HAL_GetTick();

	for (;;)
	{
		if (HAL_GetTick() - last_tick >= TICK_MS)
		{
			last_tick = HAL_GetTick();
			HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
		}
	}
	return 0;
}
