# 03-clock-c — PLL clock setup (32 MHz)

Same LED blink as [`02-struct-c`](../02-struct-c/). The only change: SYSCLK now runs at 32 MHz (STM32L1 maximum) instead of the 2.097 MHz MSI default. The new code is one `clock_init()` function called at the top of `main()`.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## Diff from 02-struct-c

**New peripheral structs** — added PWR and FLASH; RCC extended to `APB1ENR`:
```diff
+typedef struct { volatile uint32_t CR; volatile uint32_t CSR; } PWR_TypeDef;
+typedef struct { volatile uint32_t ACR; } FLASH_TypeDef;
+
 typedef struct {
     // ...
     volatile uint32_t AHBENR;   // 0x1C
+    volatile uint32_t APB2ENR;  // 0x20
+    volatile uint32_t APB1ENR;  // 0x24
 } RCC_TypeDef;
+#define PWR   ((PWR_TypeDef*)  0x40007000UL)
+#define FLASH ((FLASH_TypeDef*)0x40023C00UL)
```

**SYSCLK constant** — 2.097 MHz MSI → 32 MHz PLL:
```diff
-#define SYSCLK_HZ  2097000U
+#define SYSCLK_HZ  32000000U
```

**`main()`** — starts with a new `clock_init()`:
```diff
 int main(void) {
+    clock_init();
     RCC->AHBENR |= (1U << 1);
     // ...
 }
```

The `clock_init()` body itself is the new work — see `led_blink.c` for the seven-step PWR → FLASH → HSI → PLL sequence.

## How it works

`clock_init()` walks the sequence from RM0038. The order matters — swapping any two steps either bricks the switch or crashes the first flash fetch at the new speed.

1. **Enable the PWR peripheral clock** (`RCC->APB1ENR` bit 28). Its clock is gated off at reset; without this, writes to `PWR->CR` are silently dropped.
2. **Voltage scaling range 1** (`PWR->CR` VOS = 01). Range 1 is 1.8 V and is the only range that allows 32 MHz. Wait for `PWR->CSR` VOSF to clear — the regulator takes a moment to settle.
3. **Flash access at 32 MHz** — set `FLASH->ACR` ACC64 (64-bit reads), then PRFTEN + LATENCY = 1 (one wait state). Above 16 MHz the flash cannot answer at CPU speed without these.
4. **Turn on HSI** (16 MHz internal RC), wait for HSIRDY.
5. **Configure PLL**: PLLSRC = HSI, PLLMUL = ×4, PLLDIV = /2 → 16 × 4 / 2 = 32 MHz. The PLL must be off while writing these bits.
6. **Enable PLL** (`RCC->CR` PLLON), wait for PLLRDY.
7. **Switch SYSCLK to PLL** (`RCC->CFGR` SW = 11). Confirm by reading back SWS.

`SYSCLK_HZ` is redefined from 2 097 000 to 32 000 000. The SysTick reload becomes 31 999, still well below the 24-bit counter limit.

Build / flash / debug:
```bash
make && make flash
make debug
```

## Meaning

Every real STM32 firmware does this dance. Changing one number (the CPU clock) touches four peripherals:

- **RCC** — the clock tree itself.
- **PWR** — voltage scaling gates the maximum SYSCLK allowed.
- **FLASH** — the flash controller needs to know how fast reads come in.
- **SysTick** — its reload value depends on SYSCLK, so anything timing-related breaks when the clock changes.
