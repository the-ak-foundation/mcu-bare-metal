#ifndef __RP2040_GPIO_CFG_H__
#define __RP2040_GPIO_CFG_H__

#if __has_include("bsp_cfg.h")
#include "bsp_cfg.h"
#endif

#ifndef RP2040_GPIO_CFG_PARAM_CHECKING_ENABLE
#define RP2040_GPIO_CFG_PARAM_CHECKING_ENABLE    (BSP_CFG_PARAM_CHECKING_ENABLE)
#endif

#endif // __RP2040_GPIO_CFG_H__
