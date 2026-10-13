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

static void bsp_prv_hse_init(void)
{
	/* Enable HSE and wait until the oscillator is stable. */
	RCC->CR |= RCC_CR_HSEON;

	while (0U == (RCC->CR & RCC_CR_HSERDY))
	{
	}
}

static void bsp_prv_flash_latency_set(void)
{
	/* 0 WS for SYSCLK <= 24 MHz, 1 WS for <= 48 MHz, 2 WS for <= 72 MHz. */
	uint32_t ws = (BSP_CFG_SYSCLK_HZ > 48000000U) ? FLASH_ACR_LATENCY_2
	            : (BSP_CFG_SYSCLK_HZ > 24000000U) ? FLASH_ACR_LATENCY_1
	                                              : FLASH_ACR_LATENCY_0;

	FLASH->ACR = FLASH_ACR_PRFTBE | ws;
}

static void bsp_prv_pll_init(void)
{
	/* PLL source = HSE, PLL multiplier = BSP_CFG_PLL_MUL. SYSCLK = HSE * MUL. */
	RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL))
	          | RCC_CFGR_PLLSRC
	          | (((BSP_CFG_PLL_MUL - 2U) << RCC_CFGR_PLLMULL_Pos) & RCC_CFGR_PLLMULL);

	RCC->CR |= RCC_CR_PLLON;

	while (0U == (RCC->CR & RCC_CR_PLLRDY))
	{
	}
}

/** Initializes the system clocks: HSE -> PLL -> clk_sys 72 MHz.
 *  Sysclk switch and bus prescalers are filled in step 9. */
void bsp_clock_init(void)
{
	bsp_prv_hse_init();
	bsp_prv_flash_latency_set();
	bsp_prv_pll_init();

	SystemCoreClockUpdate();
}
