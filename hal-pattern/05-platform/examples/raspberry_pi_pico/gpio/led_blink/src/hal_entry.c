#include "board.h"
#include "bsp_delay.h"
#include "hal_data.h"

#define LED_BLINK_DELAY_MS (500)

static void hal_entry_error_trap(hal_err_t err);

void hal_entry(void)
{
	hal_err_t err;
	bsp_io_level_t    level   = BSP_IO_LEVEL_LOW;
	bsp_io_port_pin_t led_pin = g_bsp_leds.p_leds[BSP_LED_LED1];

	err = g_gpio.p_api->open(g_gpio.p_ctrl, g_gpio.p_cfg);
	hal_entry_error_trap(err);

	while (1)
	{
		if (BSP_IO_LEVEL_LOW == level)
		{
			level = BSP_IO_LEVEL_HIGH;
		}
		else
		{
			level = BSP_IO_LEVEL_LOW;
		}

		err = g_gpio.p_api->pinWrite(g_gpio.p_ctrl, led_pin, level);
		hal_entry_error_trap(err);

		BSP_SoftwareDelay(LED_BLINK_DELAY_MS, BSP_DELAY_UNITS_MILLISECONDS);
	}
}

static void hal_entry_error_trap(hal_err_t err)
{
	if (HAL_SUCCESS != err)
	{
		while (1)
		{
		}
	}
}
