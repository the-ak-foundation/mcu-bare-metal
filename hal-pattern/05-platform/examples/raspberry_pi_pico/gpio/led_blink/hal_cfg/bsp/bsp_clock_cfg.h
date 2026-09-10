#ifndef __BSP_CLOCK_CFG_H__
#define __BSP_CLOCK_CFG_H__

#define BSP_CFG_XTAL_HZ             (12000000)
#define BSP_CFG_CLOCK_SOURCE        (BSP_CLOCKS_SOURCE_CLOCK_PLL)
#define BSP_CFG_PLL_SYS_FBDIV       (125)
#define BSP_CFG_PLL_SYS_POSTDIV1    (6)
#define BSP_CFG_PLL_SYS_POSTDIV2    (2)
#define BSP_CFG_SYSCLK_HZ           (125000000)

#endif // __BSP_CLOCK_CFG_H__
