# 01-systick-c — SysTick millisecond delay

Same LED blink on PB8, but the delay is now driven by the SysTick timer instead of a busy loop. Still fully bare-metal, no HAL, no CMSIS.

## Result

<!-- Add LED blink video / gif here -->

## How it works

Reuses everything from [`00-minimal-c`](../00-minimal-c/) (vector table, `Reset_Handler`, linker script, memory-mapped I/O). What's new:

1. **Vector table grows from 2 to 16 entries.** Slot `[15]` holds `SysTick_Handler` — that slot is fixed by the ARM architecture.
2. **SysTick registers** at `0xE000_E010`:
   - `LOAD = SYSCLK_HZ / TICK_HZ - 1 = 2096` → interrupt every 1 ms (MSI is 2.097 MHz by default).
   - `CTRL = 0b111` → enable counter, enable interrupt, use SYSCLK.
3. **`SysTick_Handler`** increments `volatile uint32_t g_tick` every 1 ms.
4. **`delay_ms(ms)`** polls `g_tick`. It uses unsigned subtraction, so it stays correct when the counter wraps.
5. **`main()`** sets up GPIO and SysTick, then blinks with `delay_ms(100)`.

Build / flash / debug:
```bash
make
make flash
make debug
```

## Meaning

First example with an interrupt. Instead of "how many nops equals 100 ms?" we get real time. Introduces three things:

- The Cortex-M exception mechanism (CPU saves context and jumps to ISR).
- Precise timing that doesn't depend on CPU frequency or optimizer choices.
- Sharing data between an ISR and `main()` — hence `volatile` on `g_tick`.

Every peripheral we touch later (UART, timers, ADC) reuses the same pattern: enable clock, configure registers, handle interrupt via the vector table.
