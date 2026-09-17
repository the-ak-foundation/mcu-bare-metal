#include "hal_data.h"

rp2040_gpio_instance_ctrl_t g_gpio_ctrl;

const hal_gpio_instance_t g_gpio =
{
	.p_ctrl = &g_gpio_ctrl,
	.p_cfg  = &g_bsp_pin_cfg,
	.p_api  = &g_gpio_on_rp2040_gpio
};

rp2040_uart_instance_ctrl_t g_uart_ctrl;

static const rp2040_uart_extended_cfg_t g_uart_ext_cfg =
{
	.baud = 115200
};

static const hal_uart_cfg_t g_uart_cfg =
{
	.channel    = RP2040_UART_CHANNEL_UART0,
	.data_bits  = HAL_UART_DATA_BITS_8,
	.parity     = HAL_UART_PARITY_OFF,
	.stop_bits  = HAL_UART_STOP_BITS_1,
	.p_callback = NULL,
	.p_context  = NULL,
	.p_extend   = &g_uart_ext_cfg
};

const hal_uart_instance_t g_uart =
{
	.p_ctrl = &g_uart_ctrl,
	.p_cfg  = &g_uart_cfg,
	.p_api  = &g_uart_on_rp2040_uart
};
