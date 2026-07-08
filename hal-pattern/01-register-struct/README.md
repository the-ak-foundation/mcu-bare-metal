# 02-register-struct - Register access with `typedef struct`

Blink LED PB8 with SysTick timing (1 ms tick).

Register access groups registers by peripheral into a `typedef struct`, then maps the peripheral base address to a pointer of that struct type.

Same behavior as [`arm-cortex-m/01-systick/`](../../arm-cortex-m/01-systick/) and [`01-register-macro/`](../01-register-macro/). The difference is only in how registers are declared and used.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## Register declarations

One struct per peripheral, fields in the exact order of the RM0038 offsets:

```c
typedef struct
{
    volatile uint32_t MODER;   // 0x00
    volatile uint32_t OTYPER;  // 0x04
    volatile uint32_t OSPEEDR; // 0x08
    volatile uint32_t PUPDR;   // 0x0C
    volatile uint32_t IDR;     // 0x10
    volatile uint32_t ODR;     // 0x14
} GPIO_TypeDef;
```

Then map the base address to a pointer of that struct type:

```c
#define GPIOB ((GPIO_TypeDef*)0x40020400UL)
```

## Access

```c
RCC->AHBENR |= (1U << 1);
GPIOB->MODER |= (1U << (LED_PIN * 2));
GPIOB->ODR ^= (1U << LED_PIN);
```

The compiler computes `GPIOB->ODR` as `base + offsetof(GPIO_TypeDef, ODR)` = `0x40020400 + 0x14` = `0x40020414` — the same address as `01-register-macro`.

## Build / Flash / Debug

```bash
make
make flash
make debug
```

## Meaning

The struct captures the memory layout of the peripheral once. Every register on that peripheral is a field on the same base pointer.

Same pattern used by CMSIS device headers — see [`03-cmsis-device/`](../03-cmsis-device/), where these hand-written structs are replaced by ST's `stm32l151xb.h`.

Compare with [`01-register-macro/`](../01-register-macro/), which declares each register as a separate `#define` at a hard-coded address.
