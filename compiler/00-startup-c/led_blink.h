#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

#include <stdint.h>

/*
 * STM32 peripherals — each address = peripheral BASE + register OFFSET (RM0038)
 * RCC base   = 0x40023800   → AHBENR at offset 0x1C → 0x4002381C (RM0038 6.3.8)
 * GPIOB base = 0x40020400   → MODER  at offset 0x00 → 0x40020400 (RM0038 7.4.1)
 *                             BSRR   at offset 0x18 → 0x40020418 (RM0038 7.4.7)
 */
#define RCC_AHBENR (*(volatile uint32_t*)0x4002381C)
#define GPIOB_MODER (*(volatile uint32_t*)0x40020400)
#define GPIOB_BSRR (*(volatile uint32_t*)0x40020418)

// LED pin configuration
#define LED_PIN 8

#endif // __LED_BLINK_H__