# 03-cmsis-device - CMSIS device headers

Blink LED PB8 with SysTick timing.

This example keeps the same behavior as [`02-register-struct`](../02-register-struct/). The hand-written peripheral structs are replaced by CMSIS headers from ARM and ST.

No HAL. No ST startup files.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## Diff From 02-Register-Struct

The hand-written register structs and base pointers are replaced by one include:

```diff
-typedef struct
-{
-    volatile uint32_t MODER;
-    volatile uint32_t OTYPER;
-    volatile uint32_t OSPEEDR;
-    volatile uint32_t PUPDR;
-    volatile uint32_t IDR;
-    volatile uint32_t ODR;
-} GPIO_TypeDef;
-
-typedef struct
-{
-    volatile uint32_t CR;
-    volatile uint32_t ICSCR;
-    volatile uint32_t CFGR;
-    volatile uint32_t CIR;
-    volatile uint32_t AHBRSTR;
-    volatile uint32_t APB2RSTR;
-    volatile uint32_t APB1RSTR;
-    volatile uint32_t AHBENR;
-} RCC_TypeDef;
-
-typedef struct
-{
-    volatile uint32_t SYST_CSR;
-    volatile uint32_t SYST_RVR;
-    volatile uint32_t SYST_CVR;
-    volatile uint32_t SYST_CALIB;
-} SysTick_TypeDef;
-
-#define GPIOB   ((GPIO_TypeDef*)0x40020400UL)
-#define RCC     ((RCC_TypeDef*)0x40023800UL)
-#define SysTick ((SysTick_TypeDef*)0xE000E010UL)
+#include "stm32l1xx.h"
```

The GPIOB clock bit uses a CMSIS device macro:

```diff
-RCC->AHBENR |= (1U << 1);
+RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
```

SysTick setup uses a CMSIS-Core helper:

```diff
-SysTick->SYST_RVR = (SYSCLK_HZ / TICK_HZ) - 1U;
-SysTick->SYST_CVR = 0U;
-SysTick->SYST_CSR = (1U << 0) | (1U << 1) | (1U << 2);
+SysTick_Config(SYSCLK_HZ / TICK_HZ);
```

The Makefile adds the CMSIS include path and target chip macro:

```diff
-CFLAGS = -mcpu=cortex-m3 -mthumb ... -DAPP_START_ADDR=$(APP_START_ADDR)
+CFLAGS = -mcpu=cortex-m3 -mthumb ... -DAPP_START_ADDR=$(APP_START_ADDR) -I./cmsis -DSTM32L151xB
```

`-I./cmsis` tells the compiler where the vendored headers are.

`-DSTM32L151xB` tells `stm32l1xx.h` which STM32L1 device header to include.

## CMSIS Files

The CMSIS files are vendored in [`cmsis/`](./cmsis/).

| File | Source | Provides |
|------|--------|----------|
| `core_cm3.h` | ARM CMSIS-Core | Cortex-M3 core registers, `SysTick`, `SysTick_Config()`, `NVIC_*` |
| `cmsis_gcc.h` | ARM CMSIS-Core | GCC compiler intrinsics |
| `cmsis_compiler.h` | ARM CMSIS-Core | compiler abstraction macros |
| `cmsis_version.h` | ARM CMSIS-Core | CMSIS version macros |
| `mpu_armv7.h` | ARM CMSIS-Core | ARMv7-M MPU definitions |
| `stm32l1xx.h` | ST CMSIS-Device | device selection entry point |
| `stm32l151xb.h` | ST CMSIS-Device | STM32L151xB registers, structs, base addresses, bit masks |
| `system_stm32l1xx.h` | ST CMSIS-Device | system clock declarations |

## How It Works

`stm32l1xx.h` reads the `STM32L151xB` macro from the Makefile.

That selects `stm32l151xb.h`.

`stm32l151xb.h` defines the same kind of register structs used in `02-register-struct`, but for the full chip.

Examples:

```c
GPIOB->MODER
RCC->AHBENR
RCC_AHBENR_GPIOBEN
```

`core_cm3.h` provides Cortex-M3 core definitions such as `SysTick` and `SysTick_Config()`.

## Build / Flash / Debug

```bash
make
make flash
make debug
```

## Meaning

CMSIS replaces the hand-written register declarations.

The code still writes registers directly. There is no HAL layer here.

The difference is that register structs, base addresses, and bit masks now come from the vendor headers instead of `led_blink.h`.
