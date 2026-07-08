# 01-register-macro - Register access with `#define` macros

Blink LED PB8 with SysTick timing (1 ms tick).

Register access uses one `#define` per register, each mapped to a raw memory address.

Same behavior as [`arm-cortex-m/01-systick/`](../../arm-cortex-m/01-systick/). The difference is only in how registers are declared and used.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## Register declarations

Each register is a dereferenced pointer to a fixed address:

```c
#define RCC_AHBENR (*(volatile uint32_t*)0x4002381C)
#define GPIOB_MODER (*(volatile uint32_t*)0x40020400)
#define GPIOB_ODR (*(volatile uint32_t*)0x40020414)
```

The address comes from RM0038: peripheral base plus register offset.

## Access

```c
RCC_AHBENR |= (1U << 1);
GPIOB_MODER |= (1U << (LED_PIN * 2));
GPIOB_ODR ^= (1U << LED_PIN);
```

Each line is a raw read-modify-write on the mapped address.

## Build / Flash / Debug

```bash
make
make flash
make debug
```

## Meaning

Simplest way to touch a register from C: give it an address, cast to `volatile uint32_t*`, dereference.

Compare with [`02-register-struct/`](../02-register-struct/), which groups registers by peripheral into a struct.
