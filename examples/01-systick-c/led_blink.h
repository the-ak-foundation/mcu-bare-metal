#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

#include <stdint.h>

// STM32 peripherals — each address = peripheral BASE + register OFFSET (RM0038 §2.3)
//
// RCC base   = 0x40023800   → AHBENR at offset 0x1C → 0x4002381C
// GPIOB base = 0x40020400   → MODER  at offset 0x00 → 0x40020400
//                             BSRR   at offset 0x18 → 0x40020418
#define RCC_AHBENR  (*(volatile uint32_t*)0x4002381C)
#define GPIOB_MODER (*(volatile uint32_t*)0x40020400)
#define GPIOB_BSRR  (*(volatile uint32_t*)0x40020418)

// Cortex-M3 system registers — SysTick base = 0xE000E010 (ARMv7-M ARM §B3.3)
//   CTRL at offset 0x00 → 0xE000E010
//   LOAD at offset 0x04 → 0xE000E014
//   VAL  at offset 0x08 → 0xE000E018
#define SYSTICK_CTRL (*(volatile uint32_t*)0xE000E010)
#define SYSTICK_LOAD (*(volatile uint32_t*)0xE000E014)
#define SYSTICK_VAL  (*(volatile uint32_t*)0xE000E018)

// LED pin configuration
#define LED_PIN 8

// System clock and tick configuration
#define SYSCLK_HZ 2097000U
#define TICK_HZ 1000U

#endif // __LED_BLINK_H__