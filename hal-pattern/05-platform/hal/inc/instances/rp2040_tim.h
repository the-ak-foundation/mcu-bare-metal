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

HAL_FOOTER

#endif // __RP2040_TIM_H__
