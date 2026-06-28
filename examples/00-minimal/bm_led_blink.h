#ifndef BM_LED_BLINK_H
#define BM_LED_BLINK_H

#include <stdint.h>

#ifndef APP_START_ADDR
#error "APP_START_ADDR must be defined via -D flag in Makefile"
#endif

#define RCC_AHBENR (*(volatile uint32_t*)0x4002381C)
#define GPIOB_MODER (*(volatile uint32_t*)0x40020400)
#define GPIOB_BSRR (*(volatile uint32_t*)0x40020418)
#define SCB_VTOR (*(volatile uint32_t*)0xE000ED08)

#define LED_PIN 8

#endif