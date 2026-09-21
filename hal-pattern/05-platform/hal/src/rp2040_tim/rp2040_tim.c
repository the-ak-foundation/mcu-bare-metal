#include "rp2040_tim.h"
#include "RP2040.h"

#define RP2040_TIM_OPEN               (0x54494D45U)    /* "TIME" */
#define RP2040_TIM_CLOSED             (0x00000000U)

#define RP2040_TIM_PRV_CHANNEL_MAX    (4U)

static void     rp2040_tim_reset_unblock(void);
static uint32_t rp2040_tim_alarm_offset(uint8_t channel);
static uint32_t rp2040_tim_channel_bit(uint8_t channel);
static void     rp2040_tim_alarm_arm(uint8_t channel, uint32_t period_us);

const hal_timer_api_t g_timer_on_rp2040_tim =
{
	.open        = RP2040_TIM_Open,
	.close       = RP2040_TIM_Close,
	.start       = RP2040_TIM_Start,
	.stop        = RP2040_TIM_Stop,
	.periodSet   = RP2040_TIM_PeriodSet,
	.callbackSet = RP2040_TIM_CallbackSet
};

hal_err_t RP2040_TIM_Open(hal_timer_ctrl_t * const p_ctrl, hal_timer_cfg_t const * const p_cfg)
{
	rp2040_tim_instance_ctrl_t * p_instance_ctrl = (rp2040_tim_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ASSERT(NULL != p_cfg);
	HAL_ERROR_RETURN(p_cfg->channel < RP2040_TIM_PRV_CHANNEL_MAX, HAL_ERR_INVALID_CHANNEL);
	HAL_ERROR_RETURN(0U != p_cfg->period_us, HAL_ERR_INVALID_ARGUMENT);
	HAL_ERROR_RETURN(RP2040_TIM_OPEN != p_instance_ctrl->open, HAL_ERR_ALREADY_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	rp2040_tim_reset_unblock();

	p_instance_ctrl->channel    = p_cfg->channel;
	p_instance_ctrl->mode       = p_cfg->mode;
	p_instance_ctrl->period_us  = p_cfg->period_us;
	p_instance_ctrl->p_callback = p_cfg->p_callback;
	p_instance_ctrl->p_context  = p_cfg->p_context;
	p_instance_ctrl->open       = RP2040_TIM_OPEN;

	return HAL_SUCCESS;
}

hal_err_t RP2040_TIM_Close(hal_timer_ctrl_t * const p_ctrl)
{
	rp2040_tim_instance_ctrl_t * p_instance_ctrl = (rp2040_tim_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(RP2040_TIM_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#endif

	(void) RP2040_TIM_Stop(p_ctrl);
	p_instance_ctrl->open = RP2040_TIM_CLOSED;

	return HAL_SUCCESS;
}

hal_err_t RP2040_TIM_Start(hal_timer_ctrl_t * const p_ctrl)
{
	rp2040_tim_instance_ctrl_t * p_instance_ctrl = (rp2040_tim_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(RP2040_TIM_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#endif

	uint32_t mask = rp2040_tim_channel_bit(p_instance_ctrl->channel);

	/* Clear stale INT pending then unmask INTE for this alarm. */
	RP2040_REG(TIMER_BASE + TIMER_INTR_OFFSET) = mask;
	RP2040_REG(TIMER_BASE + REG_ALIAS_SET_BITS + TIMER_INTE_OFFSET) = mask;

	rp2040_tim_alarm_arm(p_instance_ctrl->channel, p_instance_ctrl->period_us);

	return HAL_SUCCESS;
}

hal_err_t RP2040_TIM_Stop(hal_timer_ctrl_t * const p_ctrl)
{
	rp2040_tim_instance_ctrl_t * p_instance_ctrl = (rp2040_tim_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(RP2040_TIM_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#endif

	uint32_t mask = rp2040_tim_channel_bit(p_instance_ctrl->channel);

	/* Mask INTE first so a match in flight cannot post a late IRQ.
	 * Then force-disarm via ARMED (W1C) and clear any raw pending. */
	RP2040_REG(TIMER_BASE + REG_ALIAS_CLR_BITS + TIMER_INTE_OFFSET) = mask;
	RP2040_REG(TIMER_BASE + TIMER_ARMED_OFFSET) = mask;
	RP2040_REG(TIMER_BASE + TIMER_INTR_OFFSET)  = mask;

	return HAL_SUCCESS;
}

hal_err_t RP2040_TIM_PeriodSet(hal_timer_ctrl_t * const p_ctrl, uint32_t const period_us)
{
	(void) p_ctrl;
	(void) period_us;
	return HAL_SUCCESS;
}

hal_err_t RP2040_TIM_CallbackSet(hal_timer_ctrl_t * const p_ctrl,
                                 void                 (* p_callback)(hal_timer_callback_args_t *),
                                 void * const            p_context)
{
	rp2040_tim_instance_ctrl_t * p_instance_ctrl = (rp2040_tim_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_TIM_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(RP2040_TIM_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	p_instance_ctrl->p_callback = p_callback;
	p_instance_ctrl->p_context  = p_context;

	return HAL_SUCCESS;
}

static void rp2040_tim_reset_unblock(void)
{
	uint32_t mask = RESETS_RESET_TIMER_BITS;

	RP2040_REG(RESETS_BASE + REG_ALIAS_CLR_BITS + RESETS_RESET_OFFSET) = mask;
	while ((RP2040_REG(RESETS_BASE + RESETS_RESET_DONE_OFFSET) & mask) != mask)
	{
	}
}

static uint32_t rp2040_tim_alarm_offset(uint8_t channel)
{
	return TIMER_ALARM0_OFFSET + ((uint32_t) channel * 4U);
}

static uint32_t rp2040_tim_channel_bit(uint8_t channel)
{
	return 1U << (uint32_t) channel;
}

/* Arm ALARMx = TIMERAWL + period_us. Counter runs at 1 MHz so 1 tick = 1 us. */
static void rp2040_tim_alarm_arm(uint8_t channel, uint32_t period_us)
{
	uint32_t now      = RP2040_REG(TIMER_BASE + TIMER_TIMERAWL_OFFSET);
	uint32_t deadline = now + period_us;

	RP2040_REG(TIMER_BASE + rp2040_tim_alarm_offset(channel)) = deadline;
}
