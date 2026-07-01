<div align="center">
  
![Repo Traffic](https://komarev.com/ghpvc/?username=embedded-mcu-bare-metal&label=Repo+Traffic&color=blue&style=flat-square)

</div>

# Embedded MCU Bare Metal

A collection of bare-metal examples for microcontrollers, written with hand-crafted startup code, vector tables, and linker scripts. No HAL, no vendor SDKs, no auto-generated code.

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
| [AK Embedded Base Kit](https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu) | STM32L151C8T6 (ARM Cortex-M3) | Primary target |

## Quick start

To build the source code and flash firmware onto the kit, you need to set up the development environment on Ubuntu/Linux. Step-by-step instructions are available here:

**[AK Embedded Base Kit STM32L151 — Getting Started](https://epcb.vn/blogs/ak-embedded-software/ak-embedded-base-kit-stm32l151-getting-started)**

