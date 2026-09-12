#include "bsp_compiler_support.h"
#include "bsp_delay.h"

#define BSP_DELAY_US_PER_SECOND    (1000000U)
/* Cycles per inner-loop iteration. Set to 4 as a working default;
 * verify with a scope on Cortex-M3 if delay accuracy matters. */
#define BSP_DELAY_LOOP_CYCLES      (4U)

extern uint32_t SystemCoreClock;

BSP_ATTRIBUTE_STACKLESS static void bsp_prv_software_delay_loop(uint32_t loop_count);

void BSP_SoftwareDelay(uint32_t delay, bsp_delay_units_t units)
{
	uint32_t iclk_hz        = SystemCoreClock;
	uint32_t total_us       = delay * (uint32_t) units;
	uint32_t cycles_per_us  = (iclk_hz + (BSP_DELAY_US_PER_SECOND * BSP_DELAY_LOOP_CYCLES) - 1U) /
							  (BSP_DELAY_US_PER_SECOND * BSP_DELAY_LOOP_CYCLES);
	uint64_t loops_required = (uint64_t) total_us * cycles_per_us;

	if (loops_required > UINT32_MAX)
	{
		loops_required = UINT32_MAX;
	}

	if (loops_required > 0U)
	{
		bsp_prv_software_delay_loop((uint32_t) loops_required);
	}
}

BSP_ATTRIBUTE_STACKLESS static void bsp_prv_software_delay_loop(__attribute__((unused)) uint32_t loop_count)
{
	__asm volatile(
		"1:                         \n"
		"    sub r0, r0, #1         \n"
		"    cmp r0, #0             \n"
		"    bne   1b               \n"
		"    bx lr                  \n");
}
