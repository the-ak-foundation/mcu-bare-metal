<div align="center">
  
![Repo Traffic](https://komarev.com/ghpvc/?username=embedded-mcu-bare-metal&label=Repo+Traffic&color=blue&style=flat-square)

</div>

# Embedded MCU Bare Metal

A collection of bare-metal examples for microcontrollers, written with hand-crafted startup code, vector tables, and linker scripts. No HAL, no vendor SDKs, no auto-generated code.

Every example blinks the same LED. Each one changes exactly one technique, so the diff between adjacent folders shows the concept being introduced — from raw register writes all the way up to HAL.

## Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│ APPLICATION LAYER                                                   │
│                                                                     │
│ main() · interrupt handlers · helper routines                       │
└─────────────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────────────┐
│ STARTUP                                                             │
│                                                                     │
│ Vector Table · Reset Handler                                        │
│ (interrupt vectors · BSS zero · .data copy · call main)             │
└─────────────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────────────┐
│ HARDWARE LAYER                                                      │
│                                                                     │
│ CPU Core · Interrupt Controller · System Timer                      │
│ GPIO · Clock · Flash · RAM · UART · Timer · ADC · ...               │
└─────────────────────────────────────────────────────────────────────┘
```

## Hardware support

| Board | MCU | Status |
|-------|-----|--------|
| [AK Embedded Base Kit](https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu) | STM32L151CBT6 (ARM Cortex-M3) | Primary target |

## Examples

| # | Folder | What's new |
|---|--------|-----------|
| 00 | [`00-minimal-c`](examples/00-minimal-c/) | Vector table, `Reset_Handler`, linker script, `#define` register access, busy-loop delay |
| 01 | [`01-systick-c`](examples/01-systick-c/) | SysTick interrupt for millisecond timing |
| 02 | [`02-struct-c`](examples/02-struct-c/) | `typedef struct` register access (CMSIS-style, still hand-written) |
| 03 | [`03-clock-c`](examples/03-clock-c/) | PLL clock setup — SYSCLK 32 MHz from HSI |

Planned next: `04-cmsis-core-c`, `05-cmsis-device-c`, `06-hal-c`.

## Quick start

To build the source and flash firmware onto the kit, you need a Linux development environment. Step-by-step setup instructions:

**[AK Embedded Base Kit STM32L151 — Getting Started](https://epcb.vn/blogs/ak-embedded-software/ak-embedded-base-kit-stm32l151-getting-started)**

Once the toolchain is ready, each example builds the same way:

```bash
cd examples/00-minimal-c
make          # build .elf and .bin
make flash    # program via ST-Link
make debug    # openocd + arm-none-eabi-gdb
```

## References

- [STM32World/stm32fun](https://github.com/STM32World/stm32fun) — bare-metal STM32 coding style
- STM32L1 reference manual (RM0038) and STM32L151CBT6 datasheet in [`resources/datasheet/`](resources/datasheet/)
