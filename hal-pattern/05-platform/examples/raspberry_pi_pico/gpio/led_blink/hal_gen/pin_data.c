#include "pin_data.h"
#include "rp2040_gpio.h"

const hal_gpio_pin_cfg_t g_bsp_pin_cfg_data[] =
{
	{
		.pin_cfg = RP2040_GPIO_CFG_FUNC_SIO |
				   RP2040_GPIO_CFG_DIR_OUTPUT |
				   RP2040_GPIO_CFG_OUTPUT_LOW |
				   RP2040_GPIO_CFG_PULL_NONE |
				   RP2040_GPIO_CFG_DRIVE_4MA,
		.pin     = RASPBERRY_PI_PICO_LED_PIN
	}
};

const hal_gpio_cfg_t g_bsp_pin_cfg =
{
	.number_of_pins = 1U,
	.p_pin_cfg_data = g_bsp_pin_cfg_data,
	.p_extend       = NULL
};
