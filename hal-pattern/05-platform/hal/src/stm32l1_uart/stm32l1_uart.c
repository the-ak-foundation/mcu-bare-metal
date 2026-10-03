#include "stm32l1_uart.h"

#define STM32L1_UART_OPEN               (0x55415254U)    /* "UART" */
#define STM32L1_UART_CLOSED             (0x00000000U)

#define STM32L1_UART_PRV_CHANNEL_MAX    (3U)

extern uint32_t SystemCoreClock;

static USART_TypeDef * stm32l1_uart_reg_get(uint8_t channel);
static void            stm32l1_uart_clock_enable(uint8_t channel);
static uint32_t        stm32l1_uart_pclk_get(uint8_t channel);
static hal_err_t       stm32l1_uart_reg_config(USART_TypeDef * p_reg, hal_uart_cfg_t const * p_cfg, uint32_t baud, uint32_t pclk_hz);

const hal_uart_api_t g_uart_on_stm32l1_uart =
{
	.open        = STM32L1_UART_Open,
	.close       = STM32L1_UART_Close,
	.read        = STM32L1_UART_Read,
	.write       = STM32L1_UART_Write,
	.callbackSet = STM32L1_UART_CallbackSet
};

hal_err_t STM32L1_UART_Open(hal_uart_ctrl_t * const p_ctrl, hal_uart_cfg_t const * const p_cfg)
{
	stm32l1_uart_instance_ctrl_t * p_instance_ctrl = (stm32l1_uart_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_UART_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ASSERT(NULL != p_cfg);
	HAL_ASSERT(NULL != p_cfg->p_extend);
	HAL_ERROR_RETURN(p_cfg->channel < STM32L1_UART_PRV_CHANNEL_MAX, HAL_ERR_INVALID_CHANNEL);
	HAL_ERROR_RETURN(STM32L1_UART_OPEN != p_instance_ctrl->open, HAL_ERR_ALREADY_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	stm32l1_uart_extended_cfg_t const * p_ext = (stm32l1_uart_extended_cfg_t const *) p_cfg->p_extend;

	stm32l1_uart_clock_enable(p_cfg->channel);

	USART_TypeDef * p_reg    = stm32l1_uart_reg_get(p_cfg->channel);
	uint32_t        pclk_hz  = stm32l1_uart_pclk_get(p_cfg->channel);
	hal_err_t       err      = stm32l1_uart_reg_config(p_reg, p_cfg, p_ext->baud, pclk_hz);

	HAL_ERROR_RETURN(HAL_SUCCESS == err, err);

	p_instance_ctrl->p_reg      = p_reg;
	p_instance_ctrl->channel    = p_cfg->channel;
	p_instance_ctrl->p_callback = p_cfg->p_callback;
	p_instance_ctrl->p_context  = p_cfg->p_context;
	p_instance_ctrl->open       = STM32L1_UART_OPEN;

	return HAL_SUCCESS;
}

hal_err_t STM32L1_UART_CallbackSet(hal_uart_ctrl_t * const p_ctrl,
                                   void                 (* p_callback)(hal_uart_callback_args_t *),
                                   void * const            p_context)
{
	stm32l1_uart_instance_ctrl_t * p_instance_ctrl = (stm32l1_uart_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_UART_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_UART_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	p_instance_ctrl->p_callback = p_callback;
	p_instance_ctrl->p_context  = p_context;

	return HAL_SUCCESS;
}

hal_err_t STM32L1_UART_Close(hal_uart_ctrl_t * const p_ctrl)
{
	stm32l1_uart_instance_ctrl_t * p_instance_ctrl = (stm32l1_uart_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_UART_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_UART_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	p_instance_ctrl->p_reg->CR1 = 0U;
	p_instance_ctrl->open       = STM32L1_UART_CLOSED;

	return HAL_SUCCESS;
}

hal_err_t STM32L1_UART_Write(hal_uart_ctrl_t * const p_ctrl, uint8_t const * const p_src, uint32_t const bytes)
{
	stm32l1_uart_instance_ctrl_t * p_instance_ctrl = (stm32l1_uart_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_UART_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ASSERT(NULL != p_src);
	HAL_ERROR_RETURN(0U != bytes, HAL_ERR_INVALID_SIZE);
	HAL_ERROR_RETURN(STM32L1_UART_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	USART_TypeDef * p_reg = p_instance_ctrl->p_reg;

	for (uint32_t i = 0U; i < bytes; i++)
	{
		while (0U == (p_reg->SR & USART_SR_TXE))
		{
		}
		p_reg->DR = (uint16_t) p_src[i];
	}

	while (0U == (p_reg->SR & USART_SR_TC))
	{
	}

	return HAL_SUCCESS;
}

hal_err_t STM32L1_UART_Read(hal_uart_ctrl_t * const p_ctrl, uint8_t * const p_dest, uint32_t const bytes)
{
	stm32l1_uart_instance_ctrl_t * p_instance_ctrl = (stm32l1_uart_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_UART_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ASSERT(NULL != p_dest);
	HAL_ERROR_RETURN(0U != bytes, HAL_ERR_INVALID_SIZE);
	HAL_ERROR_RETURN(STM32L1_UART_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	USART_TypeDef * p_reg = p_instance_ctrl->p_reg;

	for (uint32_t i = 0U; i < bytes; i++)
	{
		while (0U == (p_reg->SR & USART_SR_RXNE))
		{
		}
		p_dest[i] = (uint8_t) (p_reg->DR & 0xFFU);
	}

	return HAL_SUCCESS;
}

static USART_TypeDef * stm32l1_uart_reg_get(uint8_t channel)
{
	switch (channel)
	{
		case STM32L1_UART_CHANNEL_USART1:
			return USART1;
		case STM32L1_UART_CHANNEL_USART2:
			return USART2;
		case STM32L1_UART_CHANNEL_USART3:
			return USART3;
		default:
			return NULL;
	}
}

static void stm32l1_uart_clock_enable(uint8_t channel)
{
	switch (channel)
	{
		case STM32L1_UART_CHANNEL_USART1:
			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
			(void) RCC->APB2ENR;
			break;
		case STM32L1_UART_CHANNEL_USART2:
			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
			(void) RCC->APB1ENR;
			break;
		case STM32L1_UART_CHANNEL_USART3:
			RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
			(void) RCC->APB1ENR;
			break;
		default:
			break;
	}
}

static uint32_t stm32l1_uart_pclk_get(uint8_t channel)
{
	/* Valid while PCLK1_DIV = PCLK2_DIV = HCLK_DIV = 1 (see bsp_clock_cfg.h).
	 * If dividers change, read PPRE1/PPRE2 from RCC->CFGR and shift accordingly. */
	HAL_PARAMETER_NOT_USED(channel);
	return SystemCoreClock;
}

static hal_err_t stm32l1_uart_reg_config(USART_TypeDef * p_reg, hal_uart_cfg_t const * p_cfg, uint32_t baud, uint32_t pclk_hz)
{
	HAL_ERROR_RETURN(HAL_UART_DATA_BITS_7 != p_cfg->data_bits, HAL_ERR_UNSUPPORTED);
	HAL_ERROR_RETURN(0U != baud, HAL_ERR_INVALID_ARGUMENT);

	p_reg->CR1 = 0U;

	/* BRR: OVER8 = 0 (16x oversampling). Rounded division. */
	p_reg->BRR = (pclk_hz + (baud / 2U)) / baud;

	uint32_t cr1 = 0U;

	if (HAL_UART_DATA_BITS_9 == p_cfg->data_bits)
	{
		cr1 |= USART_CR1_M;
	}

	if (HAL_UART_PARITY_EVEN == p_cfg->parity)
	{
		cr1 |= USART_CR1_PCE;
	}
	else if (HAL_UART_PARITY_ODD == p_cfg->parity)
	{
		cr1 |= USART_CR1_PCE | USART_CR1_PS;
	}

	cr1 |= USART_CR1_TE | USART_CR1_RE;

	uint32_t cr2 = 0U;

	if (HAL_UART_STOP_BITS_2 == p_cfg->stop_bits)
	{
		cr2 |= USART_CR2_STOP_1;
	}

	p_reg->CR2 = cr2;
	p_reg->CR3 = 0U;

	p_reg->CR1 = cr1 | USART_CR1_UE;

	return HAL_SUCCESS;
}
