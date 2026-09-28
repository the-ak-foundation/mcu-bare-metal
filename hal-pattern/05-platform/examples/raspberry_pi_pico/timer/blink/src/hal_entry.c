#include "board.h"
#include "hal_data.h"

static void hal_entry_error_trap(hal_err_t err);

void hal_entry(void)
{
	hal_err_t err;

	err = g_gpio.p_api->open(g_gpio.p_ctrl, g_gpio.p_cfg);
	hal_entry_error_trap(err);

	err = g_timer.p_api->open(g_timer.p_ctrl, g_timer.p_cfg);
	hal_entry_error_trap(err);

	err = g_timer.p_api->start(g_timer.p_ctrl);
	hal_entry_error_trap(err);

	while (1)
	{
	}
}

void timer_blink_callback(hal_timer_callback_args_t * p_args)
{
	static bsp_io_level_t level   = BSP_IO_LEVEL_LOW;
	bsp_io_port_pin_t     led_pin = g_bsp_leds.p_leds[BSP_LED_LED1];

	HAL_PARAMETER_NOT_USED(p_args);

	if (BSP_IO_LEVEL_LOW == level)
	{
		level = BSP_IO_LEVEL_HIGH;
	}
	else
	{
		level = BSP_IO_LEVEL_LOW;
	}

	(void) g_gpio.p_api->pinWrite(g_gpio.p_ctrl, led_pin, level);
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
