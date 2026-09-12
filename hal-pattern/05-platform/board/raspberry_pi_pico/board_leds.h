#ifndef __BOARD_LEDS_H__
#define __BOARD_LEDS_H__

#include <stdint.h>

#include "hal_gpio_api.h"

HAL_HEADER

/** LED information for a board. */
typedef struct st_bsp_leds
{
	uint16_t                  led_count;
	bsp_io_port_pin_t const * p_leds;
} bsp_leds_t;

/** User LEDs available on the board. */
typedef enum e_bsp_led
{
	BSP_LED_LED1
} bsp_led_t;

extern const bsp_leds_t g_bsp_leds;

HAL_FOOTER

#endif // __BOARD_LEDS_H__
