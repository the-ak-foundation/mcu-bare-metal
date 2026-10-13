#include "bsp_clocks.h"
#include "stm32f1xx.h"

#if BSP_CFG_XTAL_HZ == 0
#error "BSP_CFG_XTAL_HZ must be greater than zero"
#endif

#if BSP_CFG_PLL_MUL == 0
#error "BSP_CFG_PLL_MUL must be greater than zero"
#endif

#if BSP_CFG_SYSCLK_HZ != (BSP_CFG_XTAL_HZ * BSP_CFG_PLL_MUL)
#error "BSP_CFG_SYSCLK_HZ does not match PLL configuration"
#endif

#if BSP_CFG_SYSCLK_HZ > 72000000
#error "STM32F103 system clock exceeds 72 MHz"
#endif

#if BSP_CFG_CLOCK_SOURCE != BSP_CLOCKS_SOURCE_CLOCK_PLL
#error "Only PLL system clock is supported"
#endif

#if BSP_CFG_PLL_SOURCE != BSP_CLOCKS_SOURCE_CLOCK_HSE
#error "Only HSE PLL source is supported"
#endif

#if (BSP_CFG_PLL_MUL < 2) || (BSP_CFG_PLL_MUL > 16)
#error "STM32F103 PLL multiplier must be in the 2..16 range"
#endif

#if (BSP_CFG_SYSCLK_HZ / BSP_CFG_APB1_DIV) > 36000000
#error "APB1 clock exceeds 36 MHz"
#endif

uint32_t SystemCoreClock = BSP_CFG_SYSCLK_HZ;

void SystemCoreClockUpdate(void)
{
	SystemCoreClock = BSP_CFG_SYSCLK_HZ;
}

/** Initializes the system clocks: HSE -> PLL -> clk_sys 72 MHz.
 *  Body filled in follow-up steps 7, 8, 9. */
void bsp_clock_init(void)
{
	SystemCoreClockUpdate();
}
