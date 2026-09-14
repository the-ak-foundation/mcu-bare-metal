#include "rp2040_uart.h"
#include "RP2040.h"

#define RP2040_UART_OPEN               (0x55415254U)    /* "UART" */
#define RP2040_UART_CLOSED             (0x00000000U)

#define RP2040_UART_PRV_CHANNEL_MAX    (2U)

static uint32_t rp2040_uart_base_get(uint8_t channel);

const hal_uart_api_t g_uart_on_rp2040_uart =
{
	.open        = RP2040_UART_Open,
	.close       = RP2040_UART_Close,
	.read        = RP2040_UART_Read,
	.write       = RP2040_UART_Write,
	.callbackSet = RP2040_UART_CallbackSet
};

hal_err_t RP2040_UART_Open(hal_uart_ctrl_t * const p_ctrl, hal_uart_cfg_t const * const p_cfg)
{
	rp2040_uart_instance_ctrl_t * p_instance_ctrl = (rp2040_uart_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_UART_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ASSERT(NULL != p_cfg);
	HAL_ASSERT(NULL != p_cfg->p_extend);
	HAL_ERROR_RETURN(p_cfg->channel < RP2040_UART_PRV_CHANNEL_MAX, HAL_ERR_INVALID_CHANNEL);
	HAL_ERROR_RETURN(RP2040_UART_OPEN != p_instance_ctrl->open, HAL_ERR_ALREADY_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	p_instance_ctrl->base       = rp2040_uart_base_get(p_cfg->channel);
	p_instance_ctrl->channel    = p_cfg->channel;
	p_instance_ctrl->p_callback = p_cfg->p_callback;
	p_instance_ctrl->p_context  = p_cfg->p_context;
	p_instance_ctrl->open       = RP2040_UART_OPEN;

	return HAL_SUCCESS;
}

hal_err_t RP2040_UART_Close(hal_uart_ctrl_t * const p_ctrl)
{
	(void) p_ctrl;
	return HAL_SUCCESS;
}

hal_err_t RP2040_UART_Read(hal_uart_ctrl_t * const p_ctrl, uint8_t * const p_dest, uint32_t const bytes)
{
	(void) p_ctrl;
	(void) p_dest;
	(void) bytes;
	return HAL_SUCCESS;
}

hal_err_t RP2040_UART_Write(hal_uart_ctrl_t * const p_ctrl, uint8_t const * const p_src, uint32_t const bytes)
{
	(void) p_ctrl;
	(void) p_src;
	(void) bytes;
	return HAL_SUCCESS;
}

hal_err_t RP2040_UART_CallbackSet(hal_uart_ctrl_t * const p_ctrl,
                                  void                 (* p_callback)(hal_uart_callback_args_t *),
                                  void * const            p_context)
{
	rp2040_uart_instance_ctrl_t * p_instance_ctrl = (rp2040_uart_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_UART_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(RP2040_UART_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	p_instance_ctrl->p_callback = p_callback;
	p_instance_ctrl->p_context  = p_context;

	return HAL_SUCCESS;
}

static uint32_t rp2040_uart_base_get(uint8_t channel)
{
	return (0U == channel) ? UART0_BASE : UART1_BASE;
}
