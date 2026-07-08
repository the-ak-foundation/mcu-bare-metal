<div align="center">
  
![Repo Traffic](https://komarev.com/ghpvc/?username=embedded-mcu-bare-metal&label=Repo+Traffic&color=blue&style=flat-square)

</div>

# Embedded MCU Bare Metal

Bare-metal microcontroller examples in three topics: compiler and linker, ARM Cortex-M core, and driver patterns (raw registers, CMSIS, HAL).

Every example blinks the same LED. Each one changes exactly one technique, so the diff between two examples in the same topic shows the new concept.

## Demo

<div align="center">
  <video src="https://github.com/user-attachments/assets/39f845b1-70a8-48ca-bb93-bfc019553a98" controls width="480"></video>
</div>

</div>

The result is the same across every example — this single clip shows what "it works" looks like for every example. Each example's own README focuses on the code change, not the video.

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

## Topics

### [`compiler/`](compiler/)

| Folder | Concept |
|--------|---------|
| [`01-startup-c/`](compiler/01-startup-c/) | Linker script and C `Reset_Handler` |

### [`arm-cortex-m/`](arm-cortex-m/)

| Folder | Concept |
|--------|---------|
| [`01-systick/`](arm-cortex-m/01-systick/) | 1 ms SysTick tick and interrupt |

### [`hal-pattern/`](hal-pattern/)

| Folder | Concept |
|--------|---------|
| [`01-register-macro/`](hal-pattern/01-register-macro/) | Macro-based register access |
| [`02-register-struct/`](hal-pattern/02-register-struct/) | Struct-based register access |
| [`03-cmsis-device/`](hal-pattern/03-cmsis-device/) | CMSIS-Device vendor headers |
| [`04-hal-blocking/`](hal-pattern/04-hal-blocking/) | HAL with blocking `HAL_Delay` |
| [`05-hal-nonblocking/`](hal-pattern/05-hal-nonblocking/) | HAL with non-blocking `HAL_GetTick` |

## Quick start

To build the source and flash firmware onto the kit, you need a Linux development environment. Step-by-step setup instructions:

**[AK Embedded Base Kit STM32L151 — Getting Started](https://epcb.vn/blogs/ak-embedded-software/ak-embedded-base-kit-stm32l151-getting-started)**

Once the toolchain is ready, each example builds the same way:

```bash
cd compiler/01-startup-c
make          # build .elf and .bin
make flash    # program via ST-Link
make debug    # openocd + arm-none-eabi-gdb
```

## References

**Datasheets & manuals** in [`resources/datasheet/`](resources/datasheet/):
- [**RM0038**](resources/datasheet/rm0038-stm32l100xx-stm32l151xx-stm32l152xx-and-stm32l162xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) — STM32L1 reference manual. Peripheral register maps (GPIO, RCC, ...). Cited in the C-startup and register-access examples.
- [**STM32L151CBT6 datasheet**](resources/datasheet/STM32L151CBT6.PDF) — pinout, package, electrical characteristics.
- [**DDI 0403**](resources/datasheet/DDI0403E_e_armv7m_arm.pdf) — ARMv7-M Architecture Reference Manual. Vector table, SysTick, NVIC, Thumb ISA. Cited in the SysTick and register-access examples.

**Related bare-metal STM32 repos:**
- [STM32World/stm32fun](https://github.com/STM32World/stm32fun) — bare-metal STM32 coding style

## Contact & Support

<p style="font-size: 20px;"><strong>Cao Trong Phuoc</strong> - Software Engineer - Embedded Systems</p>

``` Note
Thank you for visiting this repository.
If you have any questions, suggestions, or feedback about this project or firmware development, feel free to contact me directly.
```

<a href="https://github.com/caotrongphuoc">
  <img src="https://img.shields.io/badge/GitHub-caotrongphuoc-181717?style=for-the-badge&logo=github&logoColor=white"/>
</a>

<a href="https://www.linkedin.com/in/cao-trong-phuoc/">
  <img src="https://img.shields.io/badge/LinkedIn-Cao%20Trong%20Phuoc-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white"/>
</a>
