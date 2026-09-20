#ifndef __RP2040_TIM_H__
#define __RP2040_TIM_H__

#include "hal_timer_api.h"
#if __has_include("rp2040_tim_cfg.h")
#include "rp2040_tim_cfg.h"
#endif

HAL_HEADER

/** RP2040 TIM private control block. */
typedef struct st_rp2040_tim_instance_ctrl
{
	uint32_t          open;
	uint8_t           channel;      /* alarm index 0..3 */
	hal_timer_mode_t  mode;
	uint32_t          period_us;
	void           (* p_callback)(hal_timer_callback_args_t * p_args);
	void            * p_context;
} rp2040_tim_instance_ctrl_t;

/** RP2040 TIM extended configuration. Assign to hal_timer_cfg_t::p_extend. Empty placeholder for now. */
typedef struct st_rp2040_tim_extended_cfg
{
	uint32_t reserved;
} rp2040_tim_extended_cfg_t;

/** RP2040 TIM channel selector for hal_timer_cfg_t::channel. One channel = one alarm. */
typedef enum e_rp2040_tim_channel
{
	RP2040_TIM_CHANNEL_ALARM0 = 0,
	RP2040_TIM_CHANNEL_ALARM1 = 1,
	RP2040_TIM_CHANNEL_ALARM2 = 2,
	RP2040_TIM_CHANNEL_ALARM3 = 3
} rp2040_tim_channel_t;

/** Timer implementation for RP2040. */
extern const hal_timer_api_t g_timer_on_rp2040_tim;

hal_err_t RP2040_TIM_Open(hal_timer_ctrl_t * const p_ctrl, hal_timer_cfg_t const * const p_cfg);
hal_err_t RP2040_TIM_Close(hal_timer_ctrl_t * const p_ctrl);
hal_err_t RP2040_TIM_Start(hal_timer_ctrl_t * const p_ctrl);
hal_err_t RP2040_TIM_Stop(hal_timer_ctrl_t * const p_ctrl);
hal_err_t RP2040_TIM_PeriodSet(hal_timer_ctrl_t * const p_ctrl, uint32_t const period_us);
hal_err_t RP2040_TIM_CallbackSet(hal_timer_ctrl_t * const p_ctrl,
                                 void                 (* p_callback)(hal_timer_callback_args_t *),
                                 void * const            p_context);

HAL_FOOTER

#endif // __RP2040_TIM_H__
