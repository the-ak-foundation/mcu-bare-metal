#include "pin_data.h"
#include "rp2040_gpio.h"

/* UART0 on Pico: TX = GPIO0, RX = GPIO1 (pico-sdk default mapping). */
#define RASPBERRY_PI_PICO_UART_TX_PIN    ((bsp_io_port_pin_t) 0)
#define RASPBERRY_PI_PICO_UART_RX_PIN    ((bsp_io_port_pin_t) 1)

const hal_gpio_pin_cfg_t g_bsp_pin_cfg_data[] =
{
	{
		.pin_cfg = RP2040_GPIO_CFG_FUNC_UART |
				   RP2040_GPIO_CFG_PULL_NONE |
				   RP2040_GPIO_CFG_DRIVE_4MA,
		.pin     = RASPBERRY_PI_PICO_UART_TX_PIN
	},
	{
		.pin_cfg = RP2040_GPIO_CFG_FUNC_UART |
				   RP2040_GPIO_CFG_PULL_UP |
				   RP2040_GPIO_CFG_DRIVE_4MA,
		.pin     = RASPBERRY_PI_PICO_UART_RX_PIN
	}
};

const hal_gpio_cfg_t g_bsp_pin_cfg =
{
	.number_of_pins = 2U,
	.p_pin_cfg_data = g_bsp_pin_cfg_data,
	.p_extend       = NULL
};
