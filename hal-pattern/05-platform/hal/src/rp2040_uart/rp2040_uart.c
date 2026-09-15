#include "rp2040_uart.h"
#include "RP2040.h"
#include "bsp_clock_cfg.h"

#define RP2040_UART_OPEN               (0x55415254U)    /* "UART" */
#define RP2040_UART_CLOSED             (0x00000000U)

#define RP2040_UART_PRV_CHANNEL_MAX    (2U)

static uint32_t rp2040_uart_base_get(uint8_t channel);
static uint32_t rp2040_uart_reset_bit(uint8_t channel);
static void     rp2040_uart_reset_unblock(uint32_t reset_bit);
static void     rp2040_uart_baud_set(uint32_t base, uint32_t baud);
static void     rp2040_uart_line_set(uint32_t base, hal_uart_cfg_t const * p_cfg);
static void     rp2040_uart_enable(uint32_t base);

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

	rp2040_uart_extended_cfg_t const * p_ext = (rp2040_uart_extended_cfg_t const *) p_cfg->p_extend;

	uint32_t base = rp2040_uart_base_get(p_cfg->channel);

	rp2040_uart_reset_unblock(rp2040_uart_reset_bit(p_cfg->channel));
	rp2040_uart_baud_set(base, p_ext->baud);
	rp2040_uart_line_set(base, p_cfg);
	rp2040_uart_enable(base);

	p_instance_ctrl->base       = base;
	p_instance_ctrl->channel    = p_cfg->channel;
	p_instance_ctrl->p_callback = p_cfg->p_callback;
	p_instance_ctrl->p_context  = p_cfg->p_context;
	p_instance_ctrl->open       = RP2040_UART_OPEN;

	return HAL_SUCCESS;
}

hal_err_t RP2040_UART_Close(hal_uart_ctrl_t * const p_ctrl)
{
	rp2040_uart_instance_ctrl_t * p_instance_ctrl = (rp2040_uart_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_UART_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(RP2040_UART_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#endif

	/* Wait for any in-flight byte to leave the shifter before shutting off. */
	while (0U != (RP2040_REG(p_instance_ctrl->base + UART_UARTFR_OFFSET) & UART_UARTFR_BUSY_BITS))
	{
	}

	RP2040_REG(p_instance_ctrl->base + UART_UARTCR_OFFSET) = 0U;
	p_instance_ctrl->open = RP2040_UART_CLOSED;

	return HAL_SUCCESS;
}

hal_err_t RP2040_UART_Read(hal_uart_ctrl_t * const p_ctrl, uint8_t * const p_dest, uint32_t const bytes)
{
	rp2040_uart_instance_ctrl_t * p_instance_ctrl = (rp2040_uart_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_UART_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ASSERT(NULL != p_dest);
	HAL_ERROR_RETURN(0U != bytes, HAL_ERR_INVALID_ARGUMENT);
	HAL_ERROR_RETURN(RP2040_UART_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#endif

	uint32_t base = p_instance_ctrl->base;

	for (uint32_t i = 0U; i < bytes; i++)
	{
		while (0U != (RP2040_REG(base + UART_UARTFR_OFFSET) & UART_UARTFR_RXFE_BITS))
		{
		}
		p_dest[i] = (uint8_t) (RP2040_REG(base + UART_UARTDR_OFFSET) & UART_UARTDR_DATA_BITS);
	}

	return HAL_SUCCESS;
}

hal_err_t RP2040_UART_Write(hal_uart_ctrl_t * const p_ctrl, uint8_t const * const p_src, uint32_t const bytes)
{
	rp2040_uart_instance_ctrl_t * p_instance_ctrl = (rp2040_uart_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_UART_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ASSERT(NULL != p_src);
	HAL_ERROR_RETURN(0U != bytes, HAL_ERR_INVALID_ARGUMENT);
	HAL_ERROR_RETURN(RP2040_UART_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#endif

	uint32_t base = p_instance_ctrl->base;

	for (uint32_t i = 0U; i < bytes; i++)
	{
		while (0U != (RP2040_REG(base + UART_UARTFR_OFFSET) & UART_UARTFR_TXFF_BITS))
		{
		}
		RP2040_REG(base + UART_UARTDR_OFFSET) = p_src[i];
	}

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

static uint32_t rp2040_uart_reset_bit(uint8_t channel)
{
	return (0U == channel) ? RESETS_RESET_UART0_BITS : RESETS_RESET_UART1_BITS;
}

static void rp2040_uart_reset_unblock(uint32_t reset_bit)
{
	RP2040_REG(RESETS_BASE + REG_ALIAS_CLR_BITS + RESETS_RESET_OFFSET) = reset_bit;
	while ((RP2040_REG(RESETS_BASE + RESETS_RESET_DONE_OFFSET) & reset_bit) != reset_bit)
	{
	}
}

/* PL011 baud: divisor64 = 8 * clk / baud; IBRD = divisor64 / 128,
 * FBRD = ((divisor64 & 0x7F) + 1) / 2. Uses ~0.1% accuracy at common rates. */
static void rp2040_uart_baud_set(uint32_t base, uint32_t baud)
{
	uint32_t divisor64 = (8U * BSP_CFG_PERI_CLK_HZ) / baud;
	uint32_t ibrd      = divisor64 >> 7;
	uint32_t fbrd      = ((divisor64 & 0x7FU) + 1U) >> 1;

	RP2040_REG(base + UART_UARTIBRD_OFFSET) = ibrd;
	RP2040_REG(base + UART_UARTFBRD_OFFSET) = fbrd;
}

static void rp2040_uart_line_set(uint32_t base, hal_uart_cfg_t const * p_cfg)
{
	uint32_t lcr = (((uint32_t) p_cfg->data_bits - 5U) << UART_UARTLCR_H_WLEN_LSB) & UART_UARTLCR_H_WLEN_BITS;

	lcr |= UART_UARTLCR_H_FEN_BITS;

	if (HAL_UART_STOP_BITS_2 == p_cfg->stop_bits)
	{
		lcr |= UART_UARTLCR_H_STP2_BITS;
	}
	if (HAL_UART_PARITY_OFF != p_cfg->parity)
	{
		lcr |= UART_UARTLCR_H_PEN_BITS;
		if (HAL_UART_PARITY_EVEN == p_cfg->parity)
		{
			lcr |= UART_UARTLCR_H_EPS_BITS;
		}
	}

	/* LCR_H write also latches baud dividers into the shift clock. */
	RP2040_REG(base + UART_UARTLCR_H_OFFSET) = lcr;
}

static void rp2040_uart_enable(uint32_t base)
{
	RP2040_REG(base + UART_UARTCR_OFFSET) =
		UART_UARTCR_UARTEN_BITS | UART_UARTCR_TXE_BITS | UART_UARTCR_RXE_BITS;
}
