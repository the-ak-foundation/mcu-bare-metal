#include "board.h"
#include "bsp_delay.h"
#include "hal_data.h"

#define HELLO_DELAY_MS    (1000)

static const uint8_t g_hello_msg[] = "Hello, HAL\r\n";

static void hal_entry_error_trap(hal_err_t err);

void hal_entry(void)
{
	hal_err_t err;

	err = g_gpio.p_api->open(g_gpio.p_ctrl, g_gpio.p_cfg);
	hal_entry_error_trap(err);

	err = g_uart.p_api->open(g_uart.p_ctrl, g_uart.p_cfg);
	hal_entry_error_trap(err);

	while (1)
	{
		err = g_uart.p_api->write(g_uart.p_ctrl, g_hello_msg, sizeof(g_hello_msg) - 1U);
		hal_entry_error_trap(err);

		BSP_SoftwareDelay(HELLO_DELAY_MS, BSP_DELAY_UNITS_MILLISECONDS);
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
