#ifndef __HAL_DATA_H__
#define __HAL_DATA_H__

#include "pin_data.h"
#include "rp2040_gpio.h"
#include "rp2040_tim.h"

HAL_HEADER

extern rp2040_gpio_instance_ctrl_t g_gpio_ctrl;
extern const hal_gpio_instance_t   g_gpio;

extern rp2040_tim_instance_ctrl_t  g_timer_ctrl;
extern const hal_timer_instance_t  g_timer;

/** Timer overflow callback. Defined in hal_entry.c. */
void timer_blink_callback(hal_timer_callback_args_t * p_args);

void hal_entry(void);

HAL_FOOTER

#endif // __HAL_DATA_H__
