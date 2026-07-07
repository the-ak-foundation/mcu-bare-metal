# 02-cmsis-c — Use CMSIS headers from ST

Same LED blink as [`01-systick-c/struct/`](../01-systick-c/struct/). This time we drop every hand-written peripheral struct and base pointer and just `#include "stm32l1xx.h"` — the CMSIS-Core (ARM) + CMSIS-Device (ST) headers give us everything.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## Diff from 01-systick-c/struct

**Header** — 3 hand-written structs + 3 base pointers → 1 include:
```diff
-typedef struct { volatile uint32_t MODER; ...; volatile uint32_t ODR; } GPIO_TypeDef;
-typedef struct { volatile uint32_t CR; ...; volatile uint32_t AHBENR; } RCC_TypeDef;
-typedef struct { volatile uint32_t CTRL; ...; volatile uint32_t CALIB; } SysTick_TypeDef;
-#define GPIOB   ((GPIO_TypeDef*)0x40020400UL)
-#define RCC     ((RCC_TypeDef*)0x40023800UL)
-#define SysTick ((SysTick_TypeDef*)0xE000E010UL)
+#include "stm32l1xx.h"
```

**GPIOB clock enable** — raw bit → named constant from ST:
```diff
-RCC->AHBENR |= (1U << 1);
+RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
```

**SysTick setup** — 3 lines manual → 1 CMSIS-Core call:
```diff
-SysTick->LOAD = (SYSCLK_HZ / TICK_HZ) - 1U;
-SysTick->VAL = 0U;
-SysTick->CTRL = (1U << 0) | (1U << 1) | (1U << 2);
+SysTick_Config(SYSCLK_HZ / TICK_HZ);
```

**Makefile** — 2 new flags:
```diff
-CFLAGS = -mcpu=cortex-m3 -mthumb ... -DAPP_START_ADDR=$(APP_START_ADDR)
+CFLAGS = -mcpu=cortex-m3 -mthumb ... -DAPP_START_ADDR=$(APP_START_ADDR) -I./cmsis -DSTM32L151xB
```

`-I./cmsis` tells the compiler where the vendored headers live; `-DSTM32L151xB` tells `stm32l1xx.h` which chip we're building for.

## How it works

The vendored files under `cmsis/`:

| File | Source | What it provides |
|------|--------|------------------|
| `core_cm3.h`, `cmsis_gcc.h`, `cmsis_compiler.h`, `cmsis_version.h`, `mpu_armv7.h` | ARM (CMSIS-Core) | `SysTick`, `SysTick_Config()`, `NVIC_*`, compiler intrinsics |
| `stm32l1xx.h`, `stm32l151xb.h`, `system_stm32l1xx.h` | ST (CMSIS-Device) | `GPIOB`, `RCC`, all peripheral structs, `RCC_AHBENR_GPIOBEN` and every other bit-field constant |

`stm32l1xx.h` reads the `STM32L151xB` macro from the Makefile and pulls in the right device header, which in turn pulls in `core_cm3.h`. One include line reaches every symbol we need.

Build / flash / debug:
```bash
make && make flash
make debug
```

## Meaning

This is where the hand-written work pays off. Everything we crafted in `01-systick-c/struct/` (structs, base pointers, offset comments) is already sitting inside `stm32l1xx.h` in the same pattern — the ST engineers wrote it once so we don't have to. Two takeaways:

- **CMSIS is not magic.** Open `cmsis/stm32l151xb.h` and you see the same `typedef struct { volatile uint32_t MODER; ... }` we wrote by hand, plus every other register we skipped. Nothing is hidden.
- **Named constants beat raw bits.** `RCC_AHBENR_GPIOBEN` is easier to read than `(1U << 1)` and stays correct if the register layout ever changes.

## Where the CMSIS headers come from

All 8 files in [`cmsis/`](./cmsis/) are Apache-2.0 vendored copies.

- **ARM CMSIS-Core** — [ARM-software/CMSIS_5](https://github.com/ARM-software/CMSIS_5) — Cortex-M3 core helpers (`SysTick`, `NVIC_*`, compiler intrinsics).
- **ST CMSIS-Device L1** — [STMicroelectronics/cmsis_device_l1](https://github.com/STMicroelectronics/cmsis_device_l1) — STM32L1 peripheral register definitions and bit-field constants.
