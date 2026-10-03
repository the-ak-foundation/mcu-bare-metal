#include "stm32l1_tim.h"

#define STM32L1_TIM_OPEN               (0x54494D45U)    /* "TIME" */
#define STM32L1_TIM_CLOSED             (0x00000000U)

#define STM32L1_TIM_PRV_CHANNEL_MAX    (3U)
#define STM32L1_TIM_PRV_ARR_MAX        (65535U)
#define STM32L1_TIM_PRV_PSC_MAX        (65535U)
#define STM32L1_TIM_PRV_US_PER_SEC     (1000000U)

extern uint32_t SystemCoreClock;

static stm32l1_tim_instance_ctrl_t * s_ctrl[STM32L1_TIM_PRV_CHANNEL_MAX];

static TIM_TypeDef * stm32l1_tim_reg_get(uint8_t channel);
static IRQn_Type     stm32l1_tim_irq_get(uint8_t channel);
static void          stm32l1_tim_clock_enable(uint8_t channel);
static void          stm32l1_tim_clock_disable(uint8_t channel);
static uint32_t      stm32l1_tim_clock_get(uint8_t channel);
static hal_err_t     stm32l1_tim_period_compute(uint32_t period_us, uint32_t f_tim, uint16_t * p_psc, uint16_t * p_arr);
static void          stm32l1_tim_irq_dispatch(uint8_t channel);

const hal_timer_api_t g_timer_on_stm32l1_tim =
{
	.open        = STM32L1_TIM_Open,
	.close       = STM32L1_TIM_Close,
	.start       = STM32L1_TIM_Start,
	.stop        = STM32L1_TIM_Stop,
	.periodSet   = STM32L1_TIM_PeriodSet,
	.callbackSet = STM32L1_TIM_CallbackSet
};

hal_err_t STM32L1_TIM_Open(hal_timer_ctrl_t * const p_ctrl, hal_timer_cfg_t const * const p_cfg)
{
	stm32l1_tim_instance_ctrl_t * p_instance_ctrl = (stm32l1_tim_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ASSERT(NULL != p_cfg);
	HAL_ERROR_RETURN(p_cfg->channel < STM32L1_TIM_PRV_CHANNEL_MAX, HAL_ERR_INVALID_CHANNEL);
	HAL_ERROR_RETURN(0U != p_cfg->period_us, HAL_ERR_INVALID_ARGUMENT);
	HAL_ERROR_RETURN(STM32L1_TIM_OPEN != p_instance_ctrl->open, HAL_ERR_ALREADY_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	TIM_TypeDef * p_reg = stm32l1_tim_reg_get(p_cfg->channel);
	uint32_t      f_tim = stm32l1_tim_clock_get(p_cfg->channel);

	uint16_t  psc;
	uint16_t  arr;
	hal_err_t err = stm32l1_tim_period_compute(p_cfg->period_us, f_tim, &psc, &arr);
	HAL_ERROR_RETURN(HAL_SUCCESS == err, err);

	stm32l1_tim_clock_enable(p_cfg->channel);

	p_reg->CR1 = 0U;
	if (HAL_TIMER_MODE_ONE_SHOT == p_cfg->mode)
	{
		p_reg->CR1 |= TIM_CR1_OPM;
	}
	p_reg->PSC  = psc;
	p_reg->ARR  = arr;
	p_reg->CNT  = 0U;
	p_reg->EGR  = TIM_EGR_UG;
	p_reg->SR   = ~TIM_SR_UIF;
	p_reg->DIER = TIM_DIER_UIE;

	p_instance_ctrl->p_reg      = p_reg;
	p_instance_ctrl->channel    = p_cfg->channel;
	p_instance_ctrl->mode       = p_cfg->mode;
	p_instance_ctrl->p_callback = p_cfg->p_callback;
	p_instance_ctrl->p_context  = p_cfg->p_context;
	p_instance_ctrl->open       = STM32L1_TIM_OPEN;

	s_ctrl[p_cfg->channel] = p_instance_ctrl;

	NVIC_EnableIRQ(stm32l1_tim_irq_get(p_cfg->channel));

	return HAL_SUCCESS;
}

hal_err_t STM32L1_TIM_Close(hal_timer_ctrl_t * const p_ctrl)
{
	stm32l1_tim_instance_ctrl_t * p_instance_ctrl = (stm32l1_tim_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_TIM_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	uint8_t channel = p_instance_ctrl->channel;

	NVIC_DisableIRQ(stm32l1_tim_irq_get(channel));

	p_instance_ctrl->p_reg->CR1  = 0U;
	p_instance_ctrl->p_reg->DIER = 0U;

	stm32l1_tim_clock_disable(channel);

	s_ctrl[channel]       = NULL;
	p_instance_ctrl->open = STM32L1_TIM_CLOSED;

	return HAL_SUCCESS;
}

hal_err_t STM32L1_TIM_Start(hal_timer_ctrl_t * const p_ctrl)
{
	stm32l1_tim_instance_ctrl_t * p_instance_ctrl = (stm32l1_tim_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_TIM_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	p_instance_ctrl->p_reg->CNT  = 0U;
	p_instance_ctrl->p_reg->SR   = ~TIM_SR_UIF;
	p_instance_ctrl->p_reg->CR1 |= TIM_CR1_CEN;

	return HAL_SUCCESS;
}

hal_err_t STM32L1_TIM_Stop(hal_timer_ctrl_t * const p_ctrl)
{
	stm32l1_tim_instance_ctrl_t * p_instance_ctrl = (stm32l1_tim_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_TIM_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	p_instance_ctrl->p_reg->CR1 &= ~TIM_CR1_CEN;

	return HAL_SUCCESS;
}

hal_err_t STM32L1_TIM_PeriodSet(hal_timer_ctrl_t * const p_ctrl, uint32_t const period_us)
{
	stm32l1_tim_instance_ctrl_t * p_instance_ctrl = (stm32l1_tim_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_TIM_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
	HAL_ERROR_RETURN(0U != period_us, HAL_ERR_INVALID_ARGUMENT);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	uint16_t  psc;
	uint16_t  arr;
	uint32_t  f_tim = stm32l1_tim_clock_get(p_instance_ctrl->channel);
	hal_err_t err   = stm32l1_tim_period_compute(period_us, f_tim, &psc, &arr);
	HAL_ERROR_RETURN(HAL_SUCCESS == err, err);

	p_instance_ctrl->p_reg->PSC = psc;
	p_instance_ctrl->p_reg->ARR = arr;
	p_instance_ctrl->p_reg->EGR = TIM_EGR_UG;

	return HAL_SUCCESS;
}

hal_err_t STM32L1_TIM_CallbackSet(hal_timer_ctrl_t * const p_ctrl,
                                  void                 (* p_callback)(hal_timer_callback_args_t *),
                                  void * const            p_context)
{
	stm32l1_tim_instance_ctrl_t * p_instance_ctrl = (stm32l1_tim_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_TIM_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	p_instance_ctrl->p_callback = p_callback;
	p_instance_ctrl->p_context  = p_context;

	return HAL_SUCCESS;
}

static TIM_TypeDef * stm32l1_tim_reg_get(uint8_t channel)
{
	switch (channel)
	{
		case STM32L1_TIM_CHANNEL_TIM2:
			return TIM2;
		case STM32L1_TIM_CHANNEL_TIM3:
			return TIM3;
		case STM32L1_TIM_CHANNEL_TIM4:
			return TIM4;
		default:
			return NULL;
	}
}

static IRQn_Type stm32l1_tim_irq_get(uint8_t channel)
{
	switch (channel)
	{
		case STM32L1_TIM_CHANNEL_TIM2:
			return TIM2_IRQn;
		case STM32L1_TIM_CHANNEL_TIM3:
			return TIM3_IRQn;
		case STM32L1_TIM_CHANNEL_TIM4:
			return TIM4_IRQn;
		default:
			return (IRQn_Type) 0;
	}
}

static void stm32l1_tim_clock_enable(uint8_t channel)
{
	switch (channel)
	{
		case STM32L1_TIM_CHANNEL_TIM2:
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			(void) RCC->APB1ENR;
			break;
		case STM32L1_TIM_CHANNEL_TIM3:
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			(void) RCC->APB1ENR;
			break;
		case STM32L1_TIM_CHANNEL_TIM4:
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
			(void) RCC->APB1ENR;
			break;
		default:
			break;
	}
}

static void stm32l1_tim_clock_disable(uint8_t channel)
{
	switch (channel)
	{
		case STM32L1_TIM_CHANNEL_TIM2:
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
			break;
		case STM32L1_TIM_CHANNEL_TIM3:
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
			break;
		case STM32L1_TIM_CHANNEL_TIM4:
			RCC->APB1ENR &= ~RCC_APB1ENR_TIM4EN;
			break;
		default:
			break;
	}
}

static uint32_t stm32l1_tim_clock_get(uint8_t channel)
{
	/* Valid while PCLK1_DIV = HCLK_DIV = 1 (see bsp_clock_cfg.h). With APB1
	 * divider = 1 the STM32L1 timer clock equals PCLK1 (no doubling). */
	HAL_PARAMETER_NOT_USED(channel);
	return SystemCoreClock;
}

static hal_err_t stm32l1_tim_period_compute(uint32_t period_us, uint32_t f_tim, uint16_t * p_psc, uint16_t * p_arr)
{
	uint64_t total_ticks = ((uint64_t) f_tim * (uint64_t) period_us) / STM32L1_TIM_PRV_US_PER_SEC;

	HAL_ERROR_RETURN(0U != total_ticks, HAL_ERR_INVALID_ARGUMENT);

	uint32_t psc_plus_1 = (uint32_t) ((total_ticks + STM32L1_TIM_PRV_ARR_MAX) / (STM32L1_TIM_PRV_ARR_MAX + 1U));
	if (0U == psc_plus_1)
	{
		psc_plus_1 = 1U;
	}

	HAL_ERROR_RETURN(psc_plus_1 <= (STM32L1_TIM_PRV_PSC_MAX + 1U), HAL_ERR_INVALID_ARGUMENT);

	uint32_t arr_plus_1 = (uint32_t) (total_ticks / psc_plus_1);
	if (0U == arr_plus_1)
	{
		arr_plus_1 = 1U;
	}

	*p_psc = (uint16_t) (psc_plus_1 - 1U);
	*p_arr = (uint16_t) (arr_plus_1 - 1U);

	return HAL_SUCCESS;
}

static void stm32l1_tim_irq_dispatch(uint8_t channel)
{
	stm32l1_tim_instance_ctrl_t * p_instance_ctrl = s_ctrl[channel];

	if (NULL == p_instance_ctrl)
	{
		return;
	}

	TIM_TypeDef * p_reg = p_instance_ctrl->p_reg;

	if (0U == (p_reg->SR & TIM_SR_UIF))
	{
		return;
	}

	p_reg->SR = ~TIM_SR_UIF;

	if (HAL_TIMER_MODE_ONE_SHOT == p_instance_ctrl->mode)
	{
		p_reg->CR1 &= ~TIM_CR1_CEN;
	}

	if (NULL != p_instance_ctrl->p_callback)
	{
		hal_timer_callback_args_t args =
		{
			.channel   = channel,
			.event     = HAL_TIMER_EVENT_CYCLE_END,
			.p_context = p_instance_ctrl->p_context
		};
		p_instance_ctrl->p_callback(&args);
	}
}

void TIM2_IRQHandler(void)
{
	stm32l1_tim_irq_dispatch(STM32L1_TIM_CHANNEL_TIM2);
}

void TIM3_IRQHandler(void)
{
	stm32l1_tim_irq_dispatch(STM32L1_TIM_CHANNEL_TIM3);
}

void TIM4_IRQHandler(void)
{
	stm32l1_tim_irq_dispatch(STM32L1_TIM_CHANNEL_TIM4);
}
