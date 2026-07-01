#ifndef __LED_BLINK_H__
#define __LED_BLINK_H__

#include <stdint.h>

/* GPIO — RM0038 §7.4 */
typedef struct {
	volatile uint32_t MODER;   /* 0x00 */
	volatile uint32_t OTYPER;  /* 0x04 */
	volatile uint32_t OSPEEDR; /* 0x08 */
	volatile uint32_t PUPDR;   /* 0x0C */
	volatile uint32_t IDR;     /* 0x10 */
	volatile uint32_t ODR;     /* 0x14 */
	volatile uint32_t BSRR;    /* 0x18 */
} GPIO_TypeDef;

/* RCC — RM0038 §6.3 (extended to APB1ENR for PWR clock enable) */
typedef struct {
	volatile uint32_t CR;       /* 0x00: HSION[0], HSIRDY[1], PLLON[24], PLLRDY[25] */
	volatile uint32_t ICSCR;    /* 0x04 */
	volatile uint32_t CFGR;     /* 0x08: SW[1:0], SWS[3:2], PLLSRC[16], PLLMUL[21:18], PLLDIV[23:22] */
	volatile uint32_t CIR;      /* 0x0C */
	volatile uint32_t AHBRSTR;  /* 0x10 */
	volatile uint32_t APB2RSTR; /* 0x14 */
	volatile uint32_t APB1RSTR; /* 0x18 */
	volatile uint32_t AHBENR;   /* 0x1C: GPIOBEN[1] */
	volatile uint32_t APB2ENR;  /* 0x20 */
	volatile uint32_t APB1ENR;  /* 0x24: PWREN[28] */
} RCC_TypeDef;

/* PWR — RM0038 §5.4 */
typedef struct {
	volatile uint32_t CR;  /* 0x00: VOS[12:11] (01 = range 1, needed for 32 MHz) */
	volatile uint32_t CSR; /* 0x04: VOSF[4] (voltage scaling ongoing flag) */
} PWR_TypeDef;

/* FLASH — RM0038 §3.7 */
typedef struct {
	volatile uint32_t ACR; /* 0x00: LATENCY[0], PRFTEN[1], ACC64[2] */
} FLASH_TypeDef;

/* Cortex-M3 SysTick */
typedef struct {
	volatile uint32_t CTRL;
	volatile uint32_t LOAD;
	volatile uint32_t VAL;
	volatile uint32_t CALIB;
} SysTick_TypeDef;

#define GPIOB   ((GPIO_TypeDef*)   0x40020400UL)
#define RCC     ((RCC_TypeDef*)    0x40023800UL)
#define PWR     ((PWR_TypeDef*)    0x40007000UL)
#define FLASH   ((FLASH_TypeDef*)  0x40023C00UL)
#define SysTick ((SysTick_TypeDef*)0xE000E010UL)

#define LED_PIN     8
#define SYSCLK_HZ   32000000U  /* HSI 16 MHz × 4 / 2 = 32 MHz (SYSCLK max for STM32L1) */
#define TICK_HZ     1000U

#endif /* __LED_BLINK_H__ */
