#ifndef __HAL_DATA_H__
#define __HAL_DATA_H__

#include "pin_data.h"
#include "rp2040_gpio.h"
#include "rp2040_uart.h"

HAL_HEADER

extern rp2040_gpio_instance_ctrl_t g_gpio_ctrl;
extern const hal_gpio_instance_t   g_gpio;

extern rp2040_uart_instance_ctrl_t g_uart_ctrl;
extern const hal_uart_instance_t   g_uart;

void hal_entry(void);

HAL_FOOTER

#endif // __HAL_DATA_H__
