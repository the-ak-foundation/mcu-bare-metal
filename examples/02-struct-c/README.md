# 02-struct-c — Peripheral access via `typedef struct`

Same LED blink as [`01-systick-c`](../01-systick-c/). The only change is how we reach the registers: `typedef struct` + pointer instead of one `#define` per register — the same pattern CMSIS uses.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## Diff from 01-systick-c

**Header** — `#define` per register → one `typedef struct` per peripheral:
```diff
-#define GPIOB_MODER (*(volatile uint32_t*)0x40020400)
-#define GPIOB_BSRR  (*(volatile uint32_t*)0x40020418)
+typedef struct {
+    volatile uint32_t MODER;   // 0x00
+    volatile uint32_t OTYPER;  // 0x04
+    // ...
+    volatile uint32_t BSRR;    // 0x18
+} GPIO_TypeDef;
+#define GPIOB ((GPIO_TypeDef*)0x40020400UL)
```

**Access** — flat name → arrow through the struct:
```diff
-GPIOB_BSRR = (1U << 8);
+GPIOB->BSRR = (1U << 8);
```

## How it works

The compiler computes each field offset from the struct. As long as the field order and types match the datasheet layout, `GPIOB->BSRR` compiles to `*(uint32_t*)(0x40020400 + 0x18)` — the exact same address as the `#define` version.

Build / flash / debug:
```bash
make && make flash
make debug
```

## Meaning

Moving from raw addresses to a peripheral block. Why bother:

- **Scales.** GPIOA through GPIOI each have around 10 registers. One `GPIO_TypeDef` handles them all instead of hundreds of `#define` lines.
- **Offsets stay in sync.** If ST changes a layout, we fix one struct instead of hunting down individual `#define`s.
- **Same style as CMSIS.** The vendor `stm32l1xx.h` uses exactly this pattern, so `04-cmsis-device-c` will just swap our hand-written struct for the ST one.

The next examples (`03-cmsis-core-c`, `04-cmsis-device-c`, `05-hal-c`) keep climbing the same ladder — each step drops another block of hand-written code.
