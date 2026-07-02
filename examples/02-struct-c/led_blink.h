#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

#include <stdint.h>

// GPIO register block — (offset 0x00..0x18)
typedef struct
{
	volatile uint32_t MODER;   // 0x00: port mode
	volatile uint32_t OTYPER;  // 0x04: output type
	volatile uint32_t OSPEEDR; // 0x08: output speed
	volatile uint32_t PUPDR;   // 0x0C: pull-up/pull-down
	volatile uint32_t IDR;     // 0x10: input data
	volatile uint32_t ODR;     // 0x14: output data
	volatile uint32_t BSRR;    // 0x18: bit set/reset
} GPIO_TypeDef;

// RCC register block — (At AHBENR, offset 0x1C)
typedef struct
{
	volatile uint32_t CR;       // 0x00
	volatile uint32_t ICSCR;    // 0x04
	volatile uint32_t CFGR;     // 0x08
	volatile uint32_t CIR;      // 0x0C
	volatile uint32_t AHBRSTR;  // 0x10
	volatile uint32_t APB2RSTR; // 0x14
	volatile uint32_t APB1RSTR; // 0x18
	volatile uint32_t AHBENR;   // 0x1C: AHB peripheral clock enable
} RCC_TypeDef;

// Cortex-M3 SysTick
typedef struct
{
	volatile uint32_t CTRL;  // 0x00
	volatile uint32_t LOAD;  // 0x04
	volatile uint32_t VAL;   // 0x08
	volatile uint32_t CALIB; // 0x0C
} SysTick_TypeDef;

// Peripheral base pointers. Field offsets above + these bases give the same
// absolute addresses as 00-minimal-c / 01-systick-c. E.g. GPIOB->BSRR compiles
// to *(volatile uint32_t*)(0x40020400 + 0x18) = 0x40020418.
#define GPIOB   ((GPIO_TypeDef*)   0x40020400UL) // GPIOB base
#define RCC     ((RCC_TypeDef*)    0x40023800UL) // RCC base
#define SysTick ((SysTick_TypeDef*)0xE000E010UL) // SysTick base

#define LED_PIN 8
#define SYSCLK_HZ 2097000U
#define TICK_HZ 1000U

#endif // __LED_BLINK_H__
