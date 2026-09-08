#include "board.h"
#include "bsp_clocks.h"

#if defined(BOARD_RASPBERRY_PI_PICO)

/** Performs initialization specific to the Raspberry Pi Pico. */
void bsp_init(void * p_args)
{
    HAL_PARAMETER_NOT_USED(p_args);

    bsp_clock_init();
}

#endif
