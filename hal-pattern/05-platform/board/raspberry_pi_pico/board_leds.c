#include "board.h"

#if defined(BOARD_RASPBERRY_PI_PICO)

static bsp_io_port_pin_t const g_bsp_prv_leds[] =
{
	RASPBERRY_PI_PICO_LED_PIN,
};

const bsp_leds_t g_bsp_leds =
{
	.led_count = (uint16_t) (sizeof(g_bsp_prv_leds) / sizeof(g_bsp_prv_leds[0])),
	.p_leds    = &g_bsp_prv_leds[0]
};

#endif
