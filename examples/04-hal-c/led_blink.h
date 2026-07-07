#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

// stm32l1xx_hal.h pulls in stm32l1xx.h (chip register maps from CMSIS-Device)
// plus every HAL module enabled in stm32l1xx_hal_conf.h.
#include "stm32l1xx_hal.h"

#define LED_PORT GPIOB
#define LED_PIN  GPIO_PIN_8
#define TICK_MS  100U

#endif // __LED_BLINK_H__
