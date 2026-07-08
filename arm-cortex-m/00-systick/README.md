# 01-systick - SysTick LED blink

Blink LED PB8 on STM32L151CBT6 using SysTick. This example keeps the same startup code, linker script, and raw register access from [`compiler/01-startup-c`](../../compiler/01-startup-c/). The vector table grows from 2 entries to 16 to hold the SysTick slot, and the busy-loop delay is replaced by a 1 ms SysTick interrupt.

No HAL. No CMSIS. No ST startup files.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## What Changed From 01-Startup-C

### Vector Table

`01-startup-c` only needs two entries:

```c
_estack,
Reset_Handler,
```

This example adds the Cortex-M exception entries up to SysTick. SysTick is exception slot 15.

```c
__attribute__((section(".isr_vector"))) void (*const g_pfnVectors[16])(void) = {
    _estack,
    Reset_Handler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    SysTick_Handler,
};
```

### SysTick Registers

The SysTick registers are memory-mapped at `0xE000E010`.

```c
#define SYST_CSR (*(volatile uint32_t*)0xE000E010)
#define SYST_RVR (*(volatile uint32_t*)0xE000E014)
#define SYST_CVR (*(volatile uint32_t*)0xE000E018)
```

The reload value is calculated from the system clock:

```c
SYST_RVR = (SYSCLK_HZ / TICK_HZ) - 1U;
```

With:

```c
#define SYSCLK_HZ 2097000U
#define TICK_HZ 1000U
```

the timer generates one interrupt every 1 ms.

### SysTick Handler

SysTick fires every 1 ms and increments `g_tick`.

```c
volatile uint32_t g_tick;

void SysTick_Handler(void)
{
    g_tick++;
}
```

`g_tick` is `volatile` because it is written in an interrupt and read in `main()`.

### Main Loop

The LED toggles every 100 ticks.

```c
uint32_t last_tick = g_tick;

for (;;)
{
    if (g_tick - last_tick >= 100)
    {
        last_tick = g_tick;
        GPIOB_ODR ^= (1U << LED_PIN);
    }
}
```

Unsigned subtraction keeps working when `g_tick` wraps around.

## How It Works

1. `Reset_Handler` initializes `.bss` and `.data`, then calls `main()`.
2. `main()` enables the GPIOB clock.
3. `main()` configures PB8 as output.
4. `main()` configures SysTick for 1 ms interrupts.
5. `SysTick_Handler` increments `g_tick`.
6. The main loop toggles PB8 every 100 ms.

## Build / Flash / Debug

```bash
make
make flash
make debug
```

## Meaning

This example introduces the first interrupt in the series.

`01-startup-c` uses a busy-loop delay. This example uses SysTick, so timing no longer depends on the number of `nop` instructions or compiler optimization.
