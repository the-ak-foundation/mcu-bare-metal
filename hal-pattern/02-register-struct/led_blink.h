#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

#include <stdint.h>

/*
 * GPIO register block (layout from RM0038 8.4)
 * Base + field offset gives the address; the compiler does the arithmetic.
 * GPIOB base = 0x40020400
 *   MODER  → 0x40020400 + 0x00
 *   ODR    → 0x40020400 + 0x14
 */
typedef struct
{
	volatile uint32_t MODER;   // 0x00: port mode
	volatile uint32_t OTYPER;  // 0x04: output type
	volatile uint32_t OSPEEDR; // 0x08: output speed
	volatile uint32_t PUPDR;   // 0x0C: pull-up/pull-down
	volatile uint32_t IDR;     // 0x10: input data
	volatile uint32_t ODR;     // 0x14: output data
} GPIO_TypeDef;

/*
 * RCC register block (layout from RM0038 6.3)
 * RCC base = 0x40023800 → AHBENR at offset 0x1C
 */
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

// Cortex-M3 SysTick registers (ARMv7-M B3.3.2)
typedef struct
{
	volatile uint32_t SYST_CSR;   // 0x00: Control and Status Register
	volatile uint32_t SYST_RVR;   // 0x04: Reload Value Register
	volatile uint32_t SYST_CVR;   // 0x08: Current Value Register
	volatile uint32_t SYST_CALIB; // 0x0C: Calibration Value Register
} SysTick_TypeDef;

#define GPIOB ((GPIO_TypeDef*)0x40020400UL)
#define RCC ((RCC_TypeDef*)0x40023800UL)
#define SysTick ((SysTick_TypeDef*)0xE000E010UL)

// LED pin configuration
#define LED_PIN 8

// System clock and tick configuration
#define SYSCLK_HZ 2097000U
#define TICK_HZ 1000U

#endif // __LED_BLINK_H__
