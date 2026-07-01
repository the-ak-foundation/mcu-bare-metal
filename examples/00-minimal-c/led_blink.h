#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

#include <stdint.h>

// STM32 peripherals
#define RCC_AHBENR (*(volatile uint32_t*)0x4002381C)
#define GPIOB_MODER (*(volatile uint32_t*)0x40020400)
#define GPIOB_BSRR (*(volatile uint32_t*)0x40020418)

// LED pin configuration
#define LED_PIN 8

#endif // __LED_BLINK_H__