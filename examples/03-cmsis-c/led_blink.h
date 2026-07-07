#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

// One include from ST replaces all the struct + define we hand-wrote in 02.
// Picks the right chip header via the -DSTM32L151xB flag in the Makefile.
#include "stm32l1xx.h"

#define LED_PIN   8
#define SYSCLK_HZ 2097000U
#define TICK_HZ   1000U

#endif // __LED_BLINK_H__
