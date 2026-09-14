#ifndef __RP2040_UART_H__
#define __RP2040_UART_H__

#include "hal_uart_api.h"
#if __has_include("rp2040_uart_cfg.h")
#include "rp2040_uart_cfg.h"
#endif

HAL_HEADER

/** RP2040 UART private control block. */
typedef struct st_rp2040_uart_instance_ctrl
{
	uint32_t open;
	uint32_t base;   /* UART0_BASE or UART1_BASE */
	uint8_t  channel;
	void  (* p_callback)(hal_uart_callback_args_t * p_args);
	void   * p_context;
} rp2040_uart_instance_ctrl_t;

/** RP2040 UART extended configuration. Assign to hal_uart_cfg_t::p_extend. */
typedef struct st_rp2040_uart_extended_cfg
{
	uint32_t baud;   /* baud rate in bits per second, e.g. 115200 */
} rp2040_uart_extended_cfg_t;

/** RP2040 UART channel selector for hal_uart_cfg_t::channel. */
typedef enum e_rp2040_uart_channel
{
	RP2040_UART_CHANNEL_UART0 = 0,
	RP2040_UART_CHANNEL_UART1 = 1
} rp2040_uart_channel_t;

HAL_FOOTER

#endif // __RP2040_UART_H__
