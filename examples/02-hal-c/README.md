# 01-systick-c — SysTick timing (two header styles)

Same LED blink on PB8 with SysTick timing. Two variants live side by side, differing only in how peripheral registers are declared in the header:

- `macro/` — one `#define` per register.
- `struct/` — one `typedef struct` per peripheral plus a base pointer.

The `.c` file is the same in both, apart from register-access syntax.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## Layout

```
01-systick-c/
├── macro/
│   ├── led_blink.c
│   ├── led_blink.h
│   ├── Makefile
│   ├── stm32l151xx.ld
│   └── stm32l_init.gdb
├── struct/
│   ├── led_blink.c
│   ├── led_blink.h
│   ├── Makefile
│   ├── stm32l151xx.ld
│   └── stm32l_init.gdb
└── README.md
```

Each folder is self-contained. `cd` into it and build like [`00-minimal-c`](../00-minimal-c/).

## macro vs struct

Header — one `#define` per register vs. one struct per peripheral:
```diff
-#define GPIOB_MODER (*(volatile uint32_t*)0x40020400)
-#define GPIOB_ODR   (*(volatile uint32_t*)0x40020414)
+typedef struct {
+    volatile uint32_t MODER;   // 0x00
+    volatile uint32_t OTYPER;  // 0x04
+    // ...
+    volatile uint32_t ODR;     // 0x14
+} GPIO_TypeDef;
+#define GPIOB ((GPIO_TypeDef*)0x40020400UL)
```

Access — flat name vs. arrow through the struct:
```diff
-GPIOB_ODR ^= (1U << LED_PIN);
+GPIOB->ODR ^= (1U << LED_PIN);
```

`GPIOB->ODR` resolves to `0x40020400 + 0x14` — the same address as `GPIOB_ODR`. Both produce the same binary. The struct pattern is what CMSIS uses.

## Diff from 00-minimal-c

**Vector table** — 2 slots → 16, `SysTick_Handler` at [15]:
```diff
-void (*const g_pfnVectors[2])(void) = { _estack, Reset_Handler };
+void (*const g_pfnVectors[16])(void) = {
+    _estack, Reset_Handler,
+    /* [2..14] unused */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
+    SysTick_Handler,
+};
```

**Delay** — busy-loop `nop` → SysTick tick counter:
```diff
-static void delay(volatile uint32_t count) { while (count--) __asm__("nop"); }
+volatile uint32_t g_tick;
+void SysTick_Handler(void) { g_tick++; }
```

**`main()`** — added SysTick setup, then toggle ODR based on `g_tick`:
```diff
+SYST_RVR = (SYSCLK_HZ / TICK_HZ) - 1U;
+SYST_CVR = 0U;
+SYST_CSR = (1U << 0) | (1U << 1) | (1U << 2);
+
+uint32_t last_tick = g_tick;
+for (;;) {
+    if (g_tick - last_tick >= 100) {
+        last_tick = g_tick;
+        GPIOB_ODR ^= (1U << LED_PIN);
+    }
+}
```

## How it works

Reuses everything from [`00-minimal-c`](../00-minimal-c/) (vector table, `Reset_Handler`, linker script, memory-mapped I/O). What's new:

1. **Vector table grows from 2 to 16 entries.** Slot `[15]` holds `SysTick_Handler` — that slot is fixed by the ARM architecture.
2. **SysTick registers** at `0xE000_E010`:
   - `SYST_RVR = SYSCLK_HZ / TICK_HZ - 1 = 2096` → interrupt every 1 ms (MSI is 2.097 MHz by default).
   - `SYST_CSR = 0b111` → enable counter, enable interrupt, use SYSCLK.
3. **`SysTick_Handler`** increments `volatile uint32_t g_tick` every 1 ms.
4. **Toggle loop** checks `g_tick - last_tick >= 100`. Unsigned subtraction stays correct when the counter wraps.
5. **`main()`** sets up GPIO and SysTick, then runs the toggle loop.

## Build / flash / debug

```bash
cd macro          # or `cd struct`
make
make flash
make debug
```

## Meaning

First example with an interrupt. Instead of "how many nops equals 100 ms?" we get real time. Introduces three things:

- The Cortex-M exception mechanism (CPU saves context and jumps to ISR).
- Timing that doesn't depend on CPU frequency or optimizer choices.
- Sharing data between an ISR and `main()` — hence `volatile` on `g_tick`.

Every peripheral we touch later (UART, timers, ADC) reuses the same pattern: enable clock, configure registers, handle interrupt via the vector table.

The `macro/` vs `struct/` split shows two ways to declare peripherals. The next example uses the struct style, from CMSIS.
