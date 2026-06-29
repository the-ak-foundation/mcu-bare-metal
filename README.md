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