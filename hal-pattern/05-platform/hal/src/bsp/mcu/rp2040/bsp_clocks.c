#include "bsp_clocks.h"
#include "RP2040.h"

#if BSP_CFG_XTAL_HZ == 0
#error "BSP_CFG_XTAL_HZ must be greater than zero"
#endif

#if BSP_CFG_CLOCK_SOURCE != BSP_CLOCKS_SOURCE_CLOCK_PLL
#error "Only PLL system clock is supported"
#endif

#if (BSP_CFG_PLL_SYS_FBDIV < 16) || (BSP_CFG_PLL_SYS_FBDIV > 320)
#error "BSP_CFG_PLL_SYS_FBDIV must be in the 16..320 range"
#endif

#if (BSP_CFG_PLL_SYS_POSTDIV1 < 1) || (BSP_CFG_PLL_SYS_POSTDIV1 > 7)
#error "BSP_CFG_PLL_SYS_POSTDIV1 must be in the 1..7 range"
#endif

#if (BSP_CFG_PLL_SYS_POSTDIV2 < 1) || (BSP_CFG_PLL_SYS_POSTDIV2 > 7)
#error "BSP_CFG_PLL_SYS_POSTDIV2 must be in the 1..7 range"
#endif

/* PLL VCO = XTAL * FBDIV. Datasheet requires VCO in the 750..1600 MHz window. */
#if (BSP_CFG_XTAL_HZ * BSP_CFG_PLL_SYS_FBDIV) < 750000000
#error "PLL SYS VCO frequency below 750 MHz"
#endif

#if (BSP_CFG_XTAL_HZ * BSP_CFG_PLL_SYS_FBDIV) > 1600000000
#error "PLL SYS VCO frequency above 1600 MHz"
#endif

#if BSP_CFG_SYSCLK_HZ != \
	((BSP_CFG_XTAL_HZ * BSP_CFG_PLL_SYS_FBDIV) / \
	 (BSP_CFG_PLL_SYS_POSTDIV1 * BSP_CFG_PLL_SYS_POSTDIV2))
#error "BSP_CFG_SYSCLK_HZ does not match XTAL / FBDIV / POSTDIV product"
#endif

#if BSP_CFG_SYSCLK_HZ > 133000000
#error "RP2040 system clock exceeds 133 MHz"
#endif

/* System clock global, follows CMSIS convention. Read by bsp_delay.c. */
uint32_t SystemCoreClock = 6500000U;   /* ROSC default after reset */

void SystemCoreClockUpdate(void)
{
	SystemCoreClock = BSP_CFG_SYSCLK_HZ;
}

static void bsp_prv_reset_release(uint32_t mask)
{
	RP2040_REG(RESETS_BASE + REG_ALIAS_CLR_BITS + RESETS_RESET_OFFSET) = mask;
	while ((RP2040_REG(RESETS_BASE + RESETS_RESET_DONE_OFFSET) & mask) != mask)
	{
	}
}

static void bsp_prv_xosc_init(void)
{
	/* 1-15 MHz range covers Pico's 12 MHz crystal. */
	RP2040_REG(XOSC_BASE + XOSC_CTRL_OFFSET) =
		XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ << XOSC_CTRL_FREQ_RANGE_LSB;

	/* Startup delay ~1 ms: (xtal_hz / 256) * 1e-3 clocks. For 12 MHz => 47. */
	RP2040_REG(XOSC_BASE + XOSC_STARTUP_OFFSET) = 47U;

	/* Enable the oscillator. */
	RP2040_REG(XOSC_BASE + REG_ALIAS_SET_BITS + XOSC_CTRL_OFFSET) =
		XOSC_CTRL_ENABLE_VALUE_ENABLE << XOSC_CTRL_ENABLE_LSB;

	/* Wait for stable. */
	while (0U == (RP2040_REG(XOSC_BASE + XOSC_STATUS_OFFSET) & XOSC_STATUS_STABLE_BITS))
	{
	}
}

static void bsp_prv_pll_sys_init(void)
{
	bsp_prv_reset_release(RESETS_RESET_PLL_SYS_BITS);

	/* REFDIV = 1: input to PFD = xtal / REFDIV = 12 MHz. */
	RP2040_REG(PLL_SYS_BASE + PLL_CS_OFFSET) = 1U;

	/* FBDIV: VCO = PFD * FBDIV. 12 MHz * 125 = 1500 MHz. */
	RP2040_REG(PLL_SYS_BASE + PLL_FBDIV_INT_OFFSET) = BSP_CFG_PLL_SYS_FBDIV;

	/* Power up VCO and PLL. */
	RP2040_REG(PLL_SYS_BASE + REG_ALIAS_CLR_BITS + PLL_PWR_OFFSET) =
		PLL_PWR_VCOPD_BITS | PLL_PWR_PD_BITS;

	while (0U == (RP2040_REG(PLL_SYS_BASE + PLL_CS_OFFSET) & PLL_CS_LOCK_BITS))
	{
	}

	/* Post dividers: sysclk = VCO / (POSTDIV1 * POSTDIV2). 1500 / (6*2) = 125 MHz. */
	RP2040_REG(PLL_SYS_BASE + PLL_PRIM_OFFSET) =
		(BSP_CFG_PLL_SYS_POSTDIV1 << PLL_PRIM_POSTDIV1_LSB) |
		(BSP_CFG_PLL_SYS_POSTDIV2 << PLL_PRIM_POSTDIV2_LSB);

	/* Power up post divider. */
	RP2040_REG(PLL_SYS_BASE + REG_ALIAS_CLR_BITS + PLL_PWR_OFFSET) = PLL_PWR_POSTDIVPD_BITS;
}

static void bsp_prv_clk_switch(void)
{
	/* Route ref clock from XOSC. */
	RP2040_REG(CLOCKS_BASE + CLOCKS_CLK_REF_CTRL_OFFSET) =
		CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC << CLOCKS_CLK_REF_CTRL_SRC_LSB;
	while (0U == (RP2040_REG(CLOCKS_BASE + CLOCKS_CLK_REF_SELECTED_OFFSET) &
				  (1U << CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC)))
	{
	}

	/* Detach sys from aux while we change source. */
	RP2040_REG(CLOCKS_BASE + REG_ALIAS_CLR_BITS + CLOCKS_CLK_SYS_CTRL_OFFSET) =
		CLOCKS_CLK_SYS_CTRL_SRC_BITS;
	while (0U == (RP2040_REG(CLOCKS_BASE + CLOCKS_CLK_SYS_SELECTED_OFFSET) & 1U))
	{
	}

	/* Point aux at PLL_SYS then attach sys to aux. */
	RP2040_REG(CLOCKS_BASE + CLOCKS_CLK_SYS_CTRL_OFFSET) =
		(CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS << CLOCKS_CLK_SYS_CTRL_AUXSRC_LSB) |
		CLOCKS_CLK_SYS_CTRL_SRC_BITS;
	while (0U == (RP2040_REG(CLOCKS_BASE + CLOCKS_CLK_SYS_SELECTED_OFFSET) & (1U << 1)))
	{
	}

	/* Feed peripherals from clk_sys. */
	RP2040_REG(CLOCKS_BASE + CLOCKS_CLK_PERI_CTRL_OFFSET) =
		CLOCKS_CLK_PERI_CTRL_ENABLE_BITS |
		(CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS << CLOCKS_CLK_PERI_CTRL_AUXSRC_LSB);
}

/** Initializes the system clocks: XOSC -> PLL SYS -> clk_sys 125 MHz. */
void bsp_clock_init(void)
{
	/* Release IO/PADS BANK0 so GPIO drivers can touch registers later. */
	bsp_prv_reset_release(RESETS_RESET_IO_BANK0_BITS | RESETS_RESET_PADS_BANK0_BITS);

	bsp_prv_xosc_init();
	bsp_prv_pll_sys_init();
	bsp_prv_clk_switch();

	SystemCoreClockUpdate();
}
