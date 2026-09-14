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

/** UART implementation for RP2040. */
extern const hal_uart_api_t g_uart_on_rp2040_uart;

hal_err_t RP2040_UART_Open(hal_uart_ctrl_t * const p_ctrl, hal_uart_cfg_t const * const p_cfg);
hal_err_t RP2040_UART_Close(hal_uart_ctrl_t * const p_ctrl);
hal_err_t RP2040_UART_Read(hal_uart_ctrl_t * const p_ctrl, uint8_t * const p_dest, uint32_t const bytes);
hal_err_t RP2040_UART_Write(hal_uart_ctrl_t * const p_ctrl, uint8_t const * const p_src, uint32_t const bytes);
hal_err_t RP2040_UART_CallbackSet(hal_uart_ctrl_t * const p_ctrl,
                                  void                 (* p_callback)(hal_uart_callback_args_t *),
                                  void * const            p_context);

HAL_FOOTER

#endif // __RP2040_UART_H__
