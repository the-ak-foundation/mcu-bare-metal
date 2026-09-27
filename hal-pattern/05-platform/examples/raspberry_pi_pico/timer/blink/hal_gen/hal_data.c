#include "hal_data.h"

rp2040_gpio_instance_ctrl_t g_gpio_ctrl;

const hal_gpio_instance_t g_gpio =
{
	.p_ctrl = &g_gpio_ctrl,
	.p_cfg  = &g_bsp_pin_cfg,
	.p_api  = &g_gpio_on_rp2040_gpio
};

rp2040_tim_instance_ctrl_t g_timer_ctrl;

static const hal_timer_cfg_t g_timer_cfg =
{
	.channel    = RP2040_TIM_CHANNEL_ALARM0,
	.mode       = HAL_TIMER_MODE_PERIODIC,
	.period_us  = 500000U,
	.p_callback = timer_blink_callback,
	.p_context  = NULL,
	.p_extend   = NULL
};

const hal_timer_instance_t g_timer =
{
	.p_ctrl = &g_timer_ctrl,
	.p_cfg  = &g_timer_cfg,
	.p_api  = &g_timer_on_rp2040_tim
};
