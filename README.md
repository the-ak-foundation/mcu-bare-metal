<div align="center">
  
![Repo Traffic](https://ak-badge-proxy.caotrongphuoc.workers.dev/?username=embedded-mcu-bare-metal&label=Repo+Traffic&color=blue&style=flat-square)

</div>

# Embedded MCU Bare Metal

A two-part path for bare-metal MCU work.

1. **Learn** how an MCU boots and how driver code evolves, from raw registers up to a HAL.
2. **Build** a portable HAL you can apply to more than one MCU. STM32L151 on the AK Embedded Base Kit is the first target used to write and test it, then the same HAL is ported to RP2040 on the Raspberry Pi Pico to prove portability. The two targets share GPIO, UART, and Timer example sources byte-for-byte.

Every learning example blinks the same LED. Each one changes exactly one technique, so the diff between two examples in the same topic shows the new concept.

## Demo

<div align="center">
  <video src="https://github.com/user-attachments/assets/39f845b1-70a8-48ca-bb93-bfc019553a98" controls width="480"></video>
</div>

The result is the same across every learning example - this single clip shows what "it works" looks like. Each example's own README focuses on the code change, not the video.

## Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│ APPLICATION LAYER                                                   │
│                                                                     │
│ hal_entry(), interrupt handlers, helper routines                    │
└─────────────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────────────┐
│ HAL LAYER                                                           │
│                                                                     │
│ Peripheral API + instance drivers (GPIO, UART, ADC, ...)            │
│ Applications call the API vtable; instances hold the register code  │
└─────────────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────────────┐
│ BSP + STARTUP                                                       │
│                                                                     │
│ Clock init, board init, vector table, Reset_Handler                 │
│ (interrupt vectors, BSS zero, .data copy, call main)                │
└─────────────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────────────┐
│ HARDWARE LAYER                                                      │
│                                                                     │
│ CPU Core, Interrupt Controller, System Timer                        │
│ GPIO, Clock, Flash, RAM, UART, Timer, ADC, ...                      │
└─────────────────────────────────────────────────────────────────────┘
```

## Roadmap

The three topics feed each other. Read left to right for the learning path; the payoff is the HAL project at the end.

### 1. Foundations - how the MCU boots and runs

| Folder | Concept |
|:------:|:-------:|
| [`compiler/00-startup-c/`](compiler/00-startup-c/) | Linker script and C `Reset_Handler` |
| [`arm-cortex-m/00-systick/`](arm-cortex-m/00-systick/) | 1 ms SysTick tick and interrupt |

### 2. Driver-access progression - same LED, one new technique per step

| Folder | Concept |
|:------:|:-------:|
| [`hal-pattern/00-register-macro/`](hal-pattern/00-register-macro/) | Macro-based register access |
| [`hal-pattern/01-register-struct/`](hal-pattern/01-register-struct/) | Struct-based register access |
| [`hal-pattern/02-cmsis-device/`](hal-pattern/02-cmsis-device/) | CMSIS-Device vendor headers |
| [`hal-pattern/03-hal-blocking/`](hal-pattern/03-hal-blocking/) | HAL with blocking `HAL_Delay` |
| [`hal-pattern/04-hal-nonblocking/`](hal-pattern/04-hal-nonblocking/) | HAL with non-blocking `HAL_GetTick` |

### 3. The HAL - a portable project you can extend

| Folder | Concept |
|:------:|:-------:|
| [`hal-pattern/05-platform/`](hal-pattern/05-platform/) | Layered HAL project: API + instance drivers + BSP + per-project config. GPIO, UART, Timer running on STM32L151 (Cortex-M3) and RP2040 (Cortex-M0+) with byte-identical example sources |

## Hardware support

| Board | MCU | Status |
|:-----:|:---:|:------:|
| [AK Embedded Base Kit](https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu) | STM32L151CBT6 (ARM Cortex-M3) | Primary target |
| [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/) | RP2040 (dual ARM Cortex-M0+, core 0 only) | Portability target for `05-platform` |

## Quick start

Linux dev environment. Toolchain setup and board wiring:

**[AK Embedded Base Kit STM32L151 - Getting Started](https://epcb.vn/blogs/ak-embedded-software/ak-embedded-base-kit-stm32l151-getting-started)**

Each learning example builds the same way:

```bash
cd compiler/00-startup-c
make          # build .elf and .bin
make flash    # flash to board
make debug    # openocd + arm-none-eabi-gdb
```

For the HAL project (`05-platform/`), see [its README](hal-pattern/05-platform/README.md) - it uses `NAME_MODULE` and `PROJECT_DIR` to pick which example to build.

## References

**Datasheets & manuals** in [`resources/datasheet/`](resources/datasheet/):
- [**RM0038**](resources/datasheet/rm0038-stm32l100xx-stm32l151xx-stm32l152xx-and-stm32l162xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) - STM32L1 reference manual. Peripheral register maps (GPIO, RCC, ...). Cited in the C-startup and register-access examples.
- [**STM32L151CBT6 datasheet**](resources/datasheet/STM32L151CBT6.PDF) - pinout, package, electrical characteristics.
- [**DDI 0403**](resources/datasheet/DDI0403E_e_armv7m_arm.pdf) - ARMv7-M Architecture Reference Manual. Vector table, SysTick, NVIC, Thumb ISA. Cited in the SysTick and register-access examples.

**Related bare-metal STM32 repos:**
- [STM32World/stm32fun](https://github.com/STM32World/stm32fun) - bare-metal STM32 coding style

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
