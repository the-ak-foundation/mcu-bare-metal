#ifndef BM_LED_BLINK_H
#define BM_LED_BLINK_H

#include <stdint.h>

typedef struct
{
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFR[2];
	volatile uint32_t BRR;
} GPIO_TypeDef;

typedef struct
{
	volatile uint32_t CR;
	volatile uint32_t ICSCR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHBRSTR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t APB1RSTR;
	volatile uint32_t AHBENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t APB1ENR;
} RCC_TypeDef;

typedef struct
{
	volatile uint32_t CPUID;
	volatile uint32_t ICSR;
	volatile uint32_t VTOR;
} SCB_TypeDef;

#define GPIOB ((GPIO_TypeDef*)0x40020400)
#define RCC ((RCC_TypeDef*)0x40023800)
#define SCB ((SCB_TypeDef*)0xE000ED00)

#define RCC_AHBENR_GPIOBEN (1U << 1)

#define APP_START_ADDR 0x08003000U

#define LED_PORT GPIOB
#define LED_PIN 8

#endif