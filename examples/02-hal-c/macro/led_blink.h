#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

#include <stdint.h>

// RCC base   = 0x40023800   → AHBENR at offset 0x1C → 0x4002381C (RM0038 6.3.8)
// GPIOB base = 0x40020400   → MODER  at offset 0x00 → 0x40020400 (RM0038 7.4.1)
//                             ODR    at offset 0x14 → 0x40020414 (RM0038 7.4.6)
#define RCC_AHBENR (*(volatile uint32_t*)0x4002381C)
#define GPIOB_MODER (*(volatile uint32_t*)0x40020400)
#define GPIOB_ODR (*(volatile uint32_t*)0x40020414)

// Cortex-M3 SysTick registers (ARMv7-M B3.3.2)
//   SYST_CSR at 0xE000E010
//   SYST_RVR at 0xE000E014
//   SYST_CVR at 0xE000E018
#define SYST_CSR (*(volatile uint32_t*)0xE000E010)
#define SYST_RVR (*(volatile uint32_t*)0xE000E014)
#define SYST_CVR (*(volatile uint32_t*)0xE000E018)

// LED pin configuration
#define LED_PIN 8

// System clock and tick configuration
#define SYSCLK_HZ 2097000U
#define TICK_HZ 1000U

#endif // __LED_BLINK_H__
